
// qt-plus
#include "CLogger.h"

// Std
#include <algorithm>

// Qt
#include <QSet>

// Application
#include "C3DScene.h"
#include "CGeoFrame.h"
#include "CComponentFactory.h"
#include "CWorkerManager.h"
#include "CWorldTerrain.h"
#include "CWorldLodPolicy.h"
#include "CRenderLayer.h"
#include "CTreeGenerator.h"
#include "CTiledMaterial.h"
#include "CGeneratedField.h"
#include "CSRTMField.h"
#include "CHGTField.h"
#include "CBILField.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

#define LAT_MAX  90.0

//-------------------------------------------------------------------------------------------------

/*!
    \class CWorldTerrain
    \brief A dynamic terrain, with automatic LOD.
    \inmodule Quick3D
    \sa C3DScene
*/

//-------------------------------------------------------------------------------------------------

/*!
    Instantiates a new CWorldTerrain using \a pScene.
*/
CComponent* CWorldTerrain::instantiator(C3DScene* pScene)
{
    return new CWorldTerrain(pScene);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CWorldTerrain \br\br.
    \a pScene is the scene to which this terrain belongs.
    \a gCameraPosition is the geo-localization of the camera, used to build the initial terrain patches.
    \a pHeights is the height field to use for altitudes.
    \a bGenerateNow tells whether the terrain should be immediately created or threaded.
*/
CWorldTerrain::CWorldTerrain(C3DScene* pScene, CGeoloc gCameraPosition, CHeightField* pHeights, bool bGenerateNow)
    : CComponent(pScene)
    , m_bGenerateNow(bGenerateNow)
    , m_pRoot(nullptr)
    , m_pHeights(pHeights)
    , m_pMaterial(nullptr)
    , m_iLevels(15)
    , m_iTerrainResolution(31)
{
    CComponent::incComponentCounter(ClassName_CWorldTerrain);

    setName("AutoTerrain");
    setInheritTransform(false);

    m_pMaterial = pScene->ressourcesManager()->shareMaterial(QSP<CMaterial>(new CMaterial(m_pScene)));

    if (m_bGenerateNow)
    {
        LOG_METHOD_DEBUG("Generating terrain now");

        if (pScene->viewports().count() > 0 && pScene->viewports()[0]->camera())
        {
            CRenderContext context(
                        QMatrix4x4(),
                        QMatrix4x4(),
                        QMatrix4x4(),
                        QMatrix4x4(),
                        Math::CMatrix4(),
                        Math::CMatrix4(),
                        pScene,
                        pScene->viewports()[0]->camera().data()
                    );

            buildRoot(&context);
            buildRecurse(m_pRoot, &context, m_iLevels);
        }
        else
        {
            LOG_WARNING("CWorldTerrain::CWorldTerrain() : Scene has no viewport");
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CWorldTerrain.
*/
CWorldTerrain::~CWorldTerrain()
{
    clearPhysicsHeightfields();

    CComponent::decComponentCounter(ClassName_CWorldTerrain);
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the terrain resolution to \a value.
*/
void CWorldTerrain::setTerrainResolution(int value)
{
    m_iTerrainResolution = Angles::clipInt(value, 3, 81);
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this component from \a xComponent. \br\br
    \a sBaseFile is the file name from which it is loaded.
*/
void CWorldTerrain::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CComponent::loadParameters(sBaseFile, xComponent);

    m_xParameters = xComponent.getNodeByTagName(ParamName_Parameters);

    CXMLNode xGeneralNode = m_xParameters.getNodeByTagName(ParamName_General);

    m_iLevels = xGeneralNode.attributes()[ParamName_Levels].toInt();
    m_iLevels = Angles::clipInt(m_iLevels, 2, 20);

    m_iTerrainResolution = xGeneralNode.attributes()[ParamName_Resolution].toInt();
    m_iTerrainResolution = Angles::clipInt(m_iTerrainResolution, 3, 81);

    CXMLNode xFunctionsNode = m_xParameters.getNodeByTagName(ParamName_Functions);

    CXMLNode xHeightNode = m_xParameters.getNodeByTagName(ParamName_Height);

    QString sType = xHeightNode.attributes()[ParamName_Type];
    QString sPath = xHeightNode.attributes()[ParamName_Path];

    if (sType.toLower() == "srtm")
    {
        m_pHeights = new CSRTMField(m_xParameters, sPath);
    }
    else if (sType.toLower() == "hgt")
    {
        m_pHeights = new CHGTField(m_xParameters, sPath);
    }
    else if (sType.toLower() == "bil")
    {
        m_pHeights = new CBILField(m_xParameters, sPath);
    }
    else
    {
        m_pHeights = new CGeneratedField(m_xParameters);
    }

    CXMLNode xGeneratorNode = m_xParameters.getNodeByTagName(ParamName_Generators);
    QVector<CXMLNode> xComponentNodes = xGeneratorNode.getNodesByTagName(ParamName_Component);

    foreach (CXMLNode xGenerator, xComponentNodes)
    {
        QString sClass = xGenerator.attributes()[ParamName_Class];
        QSP<CComponent> pComponent(CComponentFactory::getInstance()->instantiateComponent(sClass, m_pScene));

        if (pComponent != nullptr)
        {
            QSP<CGeometryGenerator> pGenerator = QSP_CAST(CGeometryGenerator, pComponent);

            if (pGenerator != nullptr)
            {
                pGenerator->loadParameters(sBaseFile, xGenerator, xFunctionsNode);
                m_vGenerators.append(pComponent);
            }
        }
    }

    CXMLNode xMaterialNode = m_xParameters.getNodeByTagName(ParamName_Material);

    QString sMaterialType = xMaterialNode.attributes()[ParamName_Type];

    if (sMaterialType.toLower() == "blend")
    {
        m_pMaterial = m_pScene->ressourcesManager()->shareMaterial(QSP<CMaterial>(new CMaterial(m_pScene)));

        m_pMaterial->setIRFactor(0.2);

        QVector<CXMLNode> xTextures = xMaterialNode.getNodesByTagName(ParamName_Texture);

        foreach (CXMLNode xTexture, xTextures)
        {
            if (xTexture.attributes()[ParamName_Name].isEmpty() == false)
            {
                m_pMaterial->addDiffuseTexture(sBaseFile, xTexture.attributes()[ParamName_Name]);
            }
        }
    }
    else if (sMaterialType.toLower() == "bing")
    {
        CTiledMaterial* pTiled = new CTiledMaterial(m_pScene);

        pTiled->setIRFactor(0.2);
        pTiled->setLevels(m_iLevels);

        m_pMaterial = m_pScene->ressourcesManager()->shareMaterial(QSP<CMaterial>(pTiled));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Solves the links in this component after it has been loaded from an object file (XML or JSON). \br\br
    \a pScene is the scene containing this component.
*/
void CWorldTerrain::solveLinks(C3DScene* pScene)
{
    CComponent::solveLinks(pScene);

    if (m_pRoot != nullptr)
    {
        m_pRoot->solveLinks(pScene);
    }

    foreach (QSP<CComponent> pGenerator, m_vGenerators)
    {
        pGenerator->solveLinks(pScene);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Clears the links in this component and its children. \br\br
    \a pScene is the scene containing this component.
*/
void CWorldTerrain::clearLinks(C3DScene* pScene)
{
    clearPhysicsHeightfields();

    CComponent::clearLinks(pScene);

    if (m_pRoot != nullptr)
    {
        m_pRoot->clearLinks(pScene);
    }

    m_pRoot.reset();

    foreach (QSP<CComponent> pGenerator, m_vGenerators)
    {
        pGenerator->clearLinks(pScene);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Paints the terrain using \a pContext.
*/
void CWorldTerrain::paint(CRenderContext* pContext)
{
    static int iBuildCounter = 0;
    static int iGarbageCounter = 0;

    buildRoot(pContext);

    iBuildCounter++;

    if (iBuildCounter > 5)
    {
        iBuildCounter = 0;
        buildRecurse(m_pRoot, pContext, m_iLevels);
    }

    QVector<QSP<CWorldChunk> > vChunkCollect;

    paintRecurse(vChunkCollect, pContext, m_pRoot, m_iLevels);

    int iPaintLayer = (pContext != nullptr && pContext->camera() != nullptr) ? pContext->camera()->currentPaintLayer() : -1;

    QVector<SCollectedChunkInfo>& vCollected = m_mLastCollectedChunks[iPaintLayer];
    vCollected.clear();

    foreach (QSP<CWorldChunk> pCollected, vChunkCollect)
    {
        if (pCollected == nullptr) continue;

        SCollectedChunkInfo tInfo;
        tInfo.address = QString::number(quintptr(pCollected.data()), 16);
        tInfo.latitude = pCollected->geoloc().Latitude;
        tInfo.longitude = pCollected->geoloc().Longitude;
        tInfo.sizeLatitude = pCollected->size().Latitude;
        tInfo.sizeLongitude = pCollected->size().Longitude;
        vCollected.append(tInfo);
    }

    if (pContext != nullptr && pContext->camera() != nullptr)
    {
        m_gLastCameraGeoloc = pContext->camera()->geoloc();
        recordCameraPath(m_gLastCameraGeoloc);
    }

    // TEMP-PROBE partition overlap: counts chunks collected in both layer passes
    {
        static QSet<const CWorldChunk*> s_tFarCollect;
        static int s_iProbeCalls = 0;
        static bool s_bEverCollected = false;
        static int s_iEmptyPairs = 0;

        int iLayer = (pContext != nullptr && pContext->camera() != nullptr) ? pContext->camera()->currentPaintLayer() : -1;

        if (iLayer == RENDER_LAYER_FAR)
        {
            s_tFarCollect.clear();

            foreach (QSP<CWorldChunk> pChunk, vChunkCollect)
            {
                if (pChunk != nullptr) s_tFarCollect.insert(pChunk.data());
            }
        }
        else if (iLayer == RENDER_LAYER_CLOSE)
        {
            int iDuplicates = 0;

            foreach (QSP<CWorldChunk> pChunk, vChunkCollect)
            {
                if (pChunk != nullptr && s_tFarCollect.contains(pChunk.data())) iDuplicates++;
            }

            // Every empty far+close pair after the first success is a total
            // disappearance: log the event itself, sampling would miss brief ones
            if (s_tFarCollect.isEmpty() && vChunkCollect.isEmpty())
            {
                if (s_bEverCollected)
                {
                    s_iEmptyPairs++;

                    LOG_DEBUG(QString("LayerPartitionEmptyPair : count %1").arg(s_iEmptyPairs));
                }
            }
            else
            {
                s_bEverCollected = true;
            }

            s_iProbeCalls++;

            if (s_iProbeCalls % 60 == 0)
            {
                LOG_DEBUG(QString("LayerPartitionProbe : far %1 close %2 both %3 empty %4").arg(s_tFarCollect.count()).arg(vChunkCollect.count()).arg(iDuplicates).arg(s_iEmptyPairs));
            }
        }
    }

    std::sort(vChunkCollect.begin(), vChunkCollect.end());

    // Proof of single-level collect: count same-pass ground overlaps, two
    // collected chunks sharing ground mean the same zone drawn twice
    int iOverlaps = 0;

    for (int iA = 0; iA < vChunkCollect.count(); iA++)
    {
        for (int iB = iA + 1; iB < vChunkCollect.count(); iB++)
        {
            if (CWorldLodPolicy::extentsOverlap(
                        vChunkCollect[iA]->originalGeoloc(), vChunkCollect[iA]->originalSize(),
                        vChunkCollect[iB]->originalGeoloc(), vChunkCollect[iB]->originalSize()))
            {
                iOverlaps++;
            }
        }
    }

    m_mLastOverlapCount[iPaintLayer] = iOverlaps;

    foreach (QSP<CWorldChunk> pChunk, vChunkCollect)
    {
        pContext->tStatistics.m_iNumChunksDrawn++;

        pChunk->paint(pContext);
    }

    // Garbage collection

    iGarbageCounter++;

    if (iGarbageCounter > 10)
    {
        iGarbageCounter = 0;
        collectGarbage();
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates the terrain using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CWorldTerrain::update(double dDeltaTime)
{
    Q_UNUSED(dDeltaTime);

    // Note: once per frame from updateScene(), never from the per-layer
    // paint passes. Main thread only, height field bodies are never added
    // or removed during a physics step, which runs from updateScene()
    syncPhysicsHeightfields();
}

//-------------------------------------------------------------------------------------------------

/*!
    Does post update work on the terrain using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CWorldTerrain::postUpdate(double dDeltaTime)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds the root of the terrain.
*/
void CWorldTerrain::buildRoot(CRenderContext* pContext)
{
    if (!m_pRoot)
    {
        double startLat = 0.0;
        double startLon = pContext->camera()->geoloc().Longitude;
        startLon = startLon - fmod(startLon, 90.0);
        startLon = Angles::clipAngleDegreePIMinusPI(startLon);

        LOG_DEBUG(QString("CWorldTerrain::buildRoot() : camera lon = %1, start lon = %2").arg(pContext->camera()->geoloc().Longitude).arg(startLon));

        m_pRoot = QSP<CWorldChunk>(new CWorldChunk(m_pScene, this, this));
        m_pRoot->setInheritTransform(false);
        m_pRoot->CComponent::setParent(QSP<CComponent>(this));
        m_pRoot->setGeoloc(CGeoloc(startLat, startLon, 0.0));
        m_pRoot->computeWorldTransform();
        m_pRoot->setSize(CGeoloc(LAT_MAX * 2.0, 360.0, 0.0));
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true if the level of detail in \a iLevel is enough for the chunk in \a pChunk. \br\br
    \a pContext is the rendering context.
*/
bool CWorldTerrain::enoughDetail(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel)
{
    if (pChunk == nullptr || pContext == nullptr || pContext->camera() == nullptr) return true;

    return enoughDetail(pChunk, pContext->camera()->geoloc(), iLevel);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true when \a iLevel is enough detail for \a pChunk seen from \a gCamera.
*/
bool CWorldTerrain::enoughDetail(QSP<CWorldChunk> pChunk, const CGeoloc& gCamera, int iLevel)
{
    if (pChunk == nullptr) return true;

    // double dDistance = (pContext->camera()->getWorldPosition() - pChunk->getWorldBounds().center()).getMagnitude();
    // return pChunk->getWorldBounds().radius() / dDistance < 2.0;

    // if (iLevel > m_iLevels - (m_iLevels / 5)) return false;

    return (
                pChunk->worldBounds().containsSpherical(gCamera) == false ||
                iLevel == 0
                )
            && iLevel < m_iLevels;

//    return (
//                (pChunk->worldBounds().closestDistance(pContext->camera()->worldPosition()) > 2000.0) ||
//                iLevel == 0
//                )
//            && iLevel < m_iLevels;
}

//-------------------------------------------------------------------------------------------------

/*!
    Queues detail generation for \a pChunk once its terrain mesh is ready. \br\br
    The chunk worker never waits for the terrain worker itself: this build
    pass re-queues the chunk as long as its detail is missing, so a chunk
    whose worker ran too early is simply picked up again on a later pass.
*/
void CWorldTerrain::requestChunkDetailWork(QSP<CWorldChunk> pChunk)
{
    if (pChunk == nullptr) return;
    if (pChunk->isOK()) return;
    if (pChunk->terrain() == nullptr || pChunk->terrain()->isOK() == false) return;
    if (pChunk->terrain()->level() >= 2) return;

    CWorkerManager* pManager = CWorkerManager::getInstance();

    if (pManager->containsWorker(pChunk.data()) == false)
    {
        // A chunk whose worker ran before the terrain was ready is marked
        // finished: reset it so the scheduler picks it up again
        pChunk->resetForReuse();
        pManager->addWorker(pChunk.data());
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Creates the terrain and water patches of \a pChunk when missing. \br\br
    Shared by the stay branch and the covering branch of buildRecurse(): both
    need a drawable patch, the stay branch as the final level of detail, the
    covering branch as fallback cover while the finer subdivision finishes.
*/
void CWorldTerrain::ensureChunkTerrain(QSP<CWorldChunk> pChunk, int iLevel)
{
    if (pChunk == nullptr) return;

    // Create terrain for the chunk if needed
    if (pChunk->terrain() == nullptr)
    {
        CGeoloc gOriginalChunkPosition = pChunk->originalGeoloc();
        CGeoloc gOriginalChunkSize = pChunk->originalSize();

        CGeoloc gChunkPosition = pChunk->geoloc();
        CGeoloc gChunkSize = pChunk->size();

        LOG_METHOD_DEBUG(QString("Creating terrain for tile at lat %1, lon %2, level %3")
                  .arg(gChunkPosition.Latitude)
                  .arg(gChunkPosition.Longitude)
                  .arg(iLevel)
                  );

        QSP<CTerrain> pTerrain = QSP<CTerrain>(new CTerrain(
                    m_pScene,
                    m_pHeights,
                    gOriginalChunkPosition,
                    gOriginalChunkSize,
                    gChunkPosition,
                    gChunkSize,
                    m_iTerrainResolution,
                    iLevel,
                    m_iLevels,
                    false,
                    m_bGenerateNow
                    ));

        pTerrain->setMaterial(m_pMaterial);
        pTerrain->setInheritTransform(false);
        pTerrain->setGeoloc(gChunkPosition);
        pTerrain->computeWorldTransform();
        pChunk->setTerrain(pTerrain, m_bGenerateNow);

        if (m_bGenerateNow == false)
        {
            CTiledMaterial* pTiled = dynamic_cast<CTiledMaterial*>(m_pMaterial.data());

            if (pTiled == nullptr)
            {
                QSP<CTerrain> pWater = QSP<CTerrain>(new CTerrain(
                            m_pScene,
                            m_pHeights,
                            gOriginalChunkPosition,
                            gOriginalChunkSize,
                            gChunkPosition,
                            gChunkSize,
                            (int) ((double) m_iTerrainResolution * 0.75),
                            iLevel,
                            m_iLevels,
                            true,
                            m_bGenerateNow
                            ));

                pWater->setInheritTransform(false);
                pWater->setGeoloc(gChunkPosition);
                pWater->computeWorldTransform();
                pChunk->setWater(pWater);
            }
        }

        pChunk->setUsedNow();
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Tells whether \a gCamera stands inside the ground extent of \a pChunk. \br\br
    Uses the original geoloc and size, which stay in absolute coordinates: the
    transform-space geoloc and size are tile-mapped by the material and must
    not be used for coverage tests, like skipChunkForLayerPass() does.
*/
bool CWorldTerrain::isCoveringChunk(QSP<CWorldChunk> pChunk, const CGeoloc& gCamera) const
{
    if (pChunk == nullptr) return false;

    CGeoloc gCenter = pChunk->originalGeoloc();
    CGeoloc gSize = pChunk->originalSize();

    double dHalfLatitude = gSize.Latitude * 0.5;
    double dHalfLongitude = gSize.Longitude * 0.5;

    if (gCamera.Latitude < gCenter.Latitude - dHalfLatitude) return false;
    if (gCamera.Latitude > gCenter.Latitude + dHalfLatitude) return false;
    if (gCamera.Longitude < gCenter.Longitude - dHalfLongitude) return false;
    if (gCamera.Longitude > gCenter.Longitude + dHalfLongitude) return false;

    return true;
}

//-------------------------------------------------------------------------------------------------

/*!
    Called by paint() to build necessary terrain patches given the camera location. \br\br
    \a pChunk is any chunk in the chunk tree of the terrain. \br
    \a pContext is the rendering context. \br
    \a iLevel is the depth in the chunk tree.
*/
void CWorldTerrain::buildRecurse(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel)
{
    if (pChunk == nullptr) return;

    CGeoloc gOriginalChunkPosition = pChunk->originalGeoloc();
    CGeoloc gOriginalChunkSize = pChunk->originalSize();

    CGeoloc gChunkPosition = pChunk->geoloc();

    // Decide whether this level of detail is enough
    bool bStayHere = enoughDetail(pChunk, pContext, iLevel);

    {
        static int iCounter = 0;
        if (iCounter < 3)
        {
            iCounter++;
            LOG_DEBUG(QString("CWorldTerrain::buildRecurse() : level = %1, stay = %2, children = %3, has terrain = %4, cam lat = %5 lon = %6")
                .arg(iLevel).arg(bStayHere).arg(pChunk->childComponents().count()).arg(pChunk->terrain() != nullptr)
                .arg(pContext->camera()->geoloc().Latitude).arg(pContext->camera()->geoloc().Longitude));
        }
    }

    // A partial child set is never valid: subdivision creates four children,
    // removals drop the whole set. Repair here so the recreate block below
    // heals the zone through fresh builds instead of latching on coarse.
    if (pChunk->childComponents().count() > 0 && pChunk->childComponents().count() < 4)
    {
        for (int iIndex = 0; iIndex < pChunk->childComponents().count(); iIndex++)
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChunk->childComponents()[iIndex]);

            destroyChunkSubtree(pChild);

            pChunk->childComponents().remove(iIndex);
            iIndex--;
        }
    }

    if (bStayHere)
    {
        ensureChunkTerrain(pChunk, iLevel);

        requestChunkDetailWork(pChunk);
    }
    else
    {
        // A chunk covering the camera builds its own patch while the finer
        // subdivision below finishes: the paint recursion then collects the
        // closest ready ancestor instead of leaving a hole. Only the covering
        // column pays this, the far field keeps subdividing without building.
        if (pContext->camera() != nullptr && isCoveringChunk(pChunk, pContext->camera()->geoloc()))
        {
            ensureChunkTerrain(pChunk, iLevel);

            requestChunkDetailWork(pChunk);
        }

        // Create sub chunks if required
        if (pChunk->childComponents().count() == 0)
        {
            LOG_METHOD_DEBUG(QString("Creating sub-quads for tile at lat %1, lon %2, level %3")
                      .arg(gChunkPosition.Latitude)
                      .arg(gChunkPosition.Longitude)
                      .arg(iLevel)
                      );

            CGeoloc gStart(
                        gOriginalChunkPosition.Latitude - gOriginalChunkSize.Latitude * 0.50,
                        gOriginalChunkPosition.Longitude - gOriginalChunkSize.Longitude * 0.50,
                        0.0
                        );

            CGeoloc gSize(
                        gOriginalChunkSize.Latitude * 0.50,
                        gOriginalChunkSize.Longitude * 0.50,
                        0.0
                        );

//            if (iLevel == m_iLevels)
//            {
//                // Create for child chunks
//                QSP<CWorldChunk> pChild1(new CWorldChunk(m_pScene, this, this));
//                QSP<CWorldChunk> pChild2(new CWorldChunk(m_pScene, this, this));
//                QSP<CWorldChunk> pChild3(new CWorldChunk(m_pScene, this, this));
//                QSP<CWorldChunk> pChild4(new CWorldChunk(m_pScene, this, this));
//                QSP<CWorldChunk> pChild5(new CWorldChunk(m_pScene, this, this));
//                QSP<CWorldChunk> pChild6(new CWorldChunk(m_pScene, this, this));
//                QSP<CWorldChunk> pChild7(new CWorldChunk(m_pScene, this, this));
//                QSP<CWorldChunk> pChild8(new CWorldChunk(m_pScene, this, this));

//                // Children don't inherit transforms
//                pChild1->setInheritTransform(false);
//                pChild2->setInheritTransform(false);
//                pChild3->setInheritTransform(false);
//                pChild4->setInheritTransform(false);
//                pChild5->setInheritTransform(false);
//                pChild6->setInheritTransform(false);
//                pChild7->setInheritTransform(false);
//                pChild8->setInheritTransform(false);

//                // Children's parent are the given chunk
//                pChild1->CComponent::setParent(QSP<CComponent>(pChunk));
//                pChild2->CComponent::setParent(QSP<CComponent>(pChunk));
//                pChild3->CComponent::setParent(QSP<CComponent>(pChunk));
//                pChild4->CComponent::setParent(QSP<CComponent>(pChunk));
//                pChild5->CComponent::setParent(QSP<CComponent>(pChunk));
//                pChild6->CComponent::setParent(QSP<CComponent>(pChunk));
//                pChild7->CComponent::setParent(QSP<CComponent>(pChunk));
//                pChild8->CComponent::setParent(QSP<CComponent>(pChunk));

//                // Append the chlidren to the given chunk
//                pChunk->childComponents().append(pChild1);
//                pChunk->childComponents().append(pChild2);
//                pChunk->childComponents().append(pChild3);
//                pChunk->childComponents().append(pChild4);
//                pChunk->childComponents().append(pChild5);
//                pChunk->childComponents().append(pChild6);
//                pChunk->childComponents().append(pChild7);
//                pChunk->childComponents().append(pChild8);

//                // Distribute children equally in the given chunk's extents
//                pChild1->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.25, gStart.Longitude + gOriginalChunkSize.Longitude * 0.125, 0.0));
//                pChild2->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.25, gStart.Longitude + gOriginalChunkSize.Longitude * 0.375, 0.0));
//                pChild3->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.25, gStart.Longitude + gOriginalChunkSize.Longitude * 0.625, 0.0));
//                pChild4->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.25, gStart.Longitude + gOriginalChunkSize.Longitude * 0.875, 0.0));
//                pChild5->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.75, gStart.Longitude + gOriginalChunkSize.Longitude * 0.125, 0.0));
//                pChild6->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.75, gStart.Longitude + gOriginalChunkSize.Longitude * 0.375, 0.0));
//                pChild7->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.75, gStart.Longitude + gOriginalChunkSize.Longitude * 0.625, 0.0));
//                pChild8->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.75, gStart.Longitude + gOriginalChunkSize.Longitude * 0.875, 0.0));

//                // Compute transforms of children
//                pChild1->computeWorldTransform();
//                pChild2->computeWorldTransform();
//                pChild3->computeWorldTransform();
//                pChild4->computeWorldTransform();
//                pChild5->computeWorldTransform();
//                pChild6->computeWorldTransform();
//                pChild7->computeWorldTransform();
//                pChild8->computeWorldTransform();

//                // Set size of children
//                pChild1->setSize(gSize);
//                pChild2->setSize(gSize);
//                pChild3->setSize(gSize);
//                pChild4->setSize(gSize);
//                pChild5->setSize(gSize);
//                pChild6->setSize(gSize);
//                pChild7->setSize(gSize);
//                pChild8->setSize(gSize);

//                // Tell children to build themselves
//                pChild1->build();
//                pChild2->build();
//                pChild3->build();
//                pChild4->build();
//                pChild5->build();
//                pChild6->build();
//                pChild7->build();
//                pChild8->build();
//            }
//            else
            {
                // Create for child chunks
                QSP<CWorldChunk> pChild1(new CWorldChunk(m_pScene, this, this));
                QSP<CWorldChunk> pChild2(new CWorldChunk(m_pScene, this, this));
                QSP<CWorldChunk> pChild3(new CWorldChunk(m_pScene, this, this));
                QSP<CWorldChunk> pChild4(new CWorldChunk(m_pScene, this, this));

                // Children don't inherit transforms
                pChild1->setInheritTransform(false);
                pChild2->setInheritTransform(false);
                pChild3->setInheritTransform(false);
                pChild4->setInheritTransform(false);

                // Children's parent are the given chunk
                pChild1->CComponent::setParent(QSP<CComponent>(pChunk));
                pChild2->CComponent::setParent(QSP<CComponent>(pChunk));
                pChild3->CComponent::setParent(QSP<CComponent>(pChunk));
                pChild4->CComponent::setParent(QSP<CComponent>(pChunk));

                // Append the chlidren to the given chunk
                pChunk->childComponents().append(pChild1);
                pChunk->childComponents().append(pChild2);
                pChunk->childComponents().append(pChild3);
                pChunk->childComponents().append(pChild4);

                // Distribute children equally in the given chunk's extents
                pChild1->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.25, gStart.Longitude + gOriginalChunkSize.Longitude * 0.25, 0.0));
                pChild2->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.25, gStart.Longitude + gOriginalChunkSize.Longitude * 0.75, 0.0));
                pChild3->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.75, gStart.Longitude + gOriginalChunkSize.Longitude * 0.25, 0.0));
                pChild4->setGeoloc(CGeoloc(gStart.Latitude + gOriginalChunkSize.Latitude * 0.75, gStart.Longitude + gOriginalChunkSize.Longitude * 0.75, 0.0));

                // Compute transforms of children
                pChild1->computeWorldTransform();
                pChild2->computeWorldTransform();
                pChild3->computeWorldTransform();
                pChild4->computeWorldTransform();

                // Set size of children
                pChild1->setSize(gSize);
                pChild2->setSize(gSize);
                pChild3->setSize(gSize);
                pChild4->setSize(gSize);

                // Tell children to build themselves
                pChild1->build();
                pChild2->build();
                pChild3->build();
                pChild4->build();
            }
        }

        // Recurse in child chunks
        foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            buildRecurse(pChild, pContext, iLevel - 1);
        }
    }
}

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------

//! Depth margin covering point-sprite bushes, culled on the generous chunk box instead of their own bounds
static const double LayerPartitionVegetationMargin = 5.0;

/***************************************************************************/
/*!
    Expands the camera-space depth interval [\a dDepthMin, \a dDepthMax] with the
    sphere centered on \a vCenter with radius \a dRadius. \br\br
    \a mCameraMatrix is the render context internal camera matrix, the same frame
    the per-geometry draw test uses, so the forward axis matches the layer depth
    planes exactly.
*/
/***************************************************************************/
static void expandLayerDepthInterval(double& dDepthMin, double& dDepthMax, const Math::CMatrix4& mCameraMatrix, const CVector3& vCenter, double dRadius)
{
    CVector3 vPosition = mCameraMatrix * vCenter;

    if (vPosition.Z - dRadius < dDepthMin) dDepthMin = vPosition.Z - dRadius;
    if (vPosition.Z + dRadius > dDepthMax) dDepthMax = vPosition.Z + dRadius;
}

//-------------------------------------------------------------------------------------------------

/*!
    Tells whether \a pChunk contributes nothing to the layer pass in \a pContext. \br\br
    The interval is the camera-space forward depth of the chunk in the same frame the
    per-geometry draw test uses (CMeshGeometry::paint through CCamera::contains()):
    the layer depth planes clip on this axis, so collect and drawing speak the same
    language. In the far pass chunks fully inside the close range are skipped, in the
    close pass chunks fully outside it are skipped. Chunks spanning the close far plane
    are kept in both passes.
*/
bool CWorldTerrain::skipChunkForLayerPass(QSP<CWorldChunk> pChunk, CRenderContext* pContext)
{
    if (pChunk == nullptr || pContext == nullptr || pContext->camera() == nullptr) return false;

    int iLayer = pContext->camera()->currentPaintLayer();

    if (iLayer != RENDER_LAYER_FAR && iLayer != RENDER_LAYER_CLOSE) return false;

    double dCloseFar = pContext->camera()->closeLayerFar();

    if (dCloseFar <= 0.0) return false;

    Math::CMatrix4 mCameraMatrix = pContext->internalCameraMatrix();

    double dDepthMin = Q3D_INFINITY;
    double dDepthMax = -Q3D_INFINITY;

    // The transform-space geoloc/size are tile-mapped by the material and must
    // not be used here: the original geoloc/size stay in absolute coordinates.
    // This surface sphere covers unbuilt chunks, whose exact bounds are unknown
    CGeoloc gCenter = pChunk->originalGeoloc();
    CGeoloc gSize = pChunk->originalSize();

    CGeoloc gCornerA(
            gCenter.Latitude - gSize.Latitude * 0.5,
            gCenter.Longitude - gSize.Longitude * 0.5,
            gCenter.Altitude);
    CGeoloc gCornerB(
            gCenter.Latitude + gSize.Latitude * 0.5,
            gCenter.Longitude + gSize.Longitude * 0.5,
            gCenter.Altitude);

    expandLayerDepthInterval(
                dDepthMin,
                dDepthMax,
                mCameraMatrix,
                gCenter.toVector3(),
                gCornerA.distanceTo(gCornerB) * 0.5 + LayerPartitionVegetationMargin
                );

    // Exact bounds the drawing uses, covering relief the surface sphere ignores
    if (pChunk->terrain() != nullptr && pChunk->terrain()->isOK())
    {
        CBoundingBox tBounds = pChunk->terrain()->worldBounds();

        expandLayerDepthInterval(dDepthMin, dDepthMax, mCameraMatrix, tBounds.center(), tBounds.radius());
    }

    if (pChunk->water() != nullptr && pChunk->water()->isOK())
    {
        CBoundingBox tBounds = pChunk->water()->worldBounds();

        expandLayerDepthInterval(dDepthMin, dDepthMax, mCameraMatrix, tBounds.center(), tBounds.radius());
    }

    foreach (CBoundedMeshInstances* pBoundedMeshes, pChunk->meshes())
    {
        if (pBoundedMeshes != nullptr)
        {
            CBoundingBox tBounds = pBoundedMeshes->worldBounds();

            expandLayerDepthInterval(dDepthMin, dDepthMax, mCameraMatrix, tBounds.center(), tBounds.radius());
        }
    }

    if (iLayer == RENDER_LAYER_FAR)
    {
        return dDepthMax < dCloseFar;
    }

    return dDepthMin > dCloseFar;
}

//-------------------------------------------------------------------------------------------------

/*!
    Records the quadtree readiness on the path from the root to \a gCamera, retained for snapshots. \br\br
    Descent follows the child containing the camera like the paint recursion does:
    the first level with no ready chunk and no drawable child on this path is
    where the frame gives up and falls back.
*/
void CWorldTerrain::recordCameraPath(const CGeoloc& gCamera)
{
    m_vLastCameraPath.clear();

    QSP<CWorldChunk> pChunk = m_pRoot;
    int iLevel = m_iLevels;

    while (pChunk != nullptr && iLevel >= 0)
    {
        SChunkPathInfo tInfo;
        tInfo.level = iLevel;
        tInfo.latitude = pChunk->geoloc().Latitude;
        tInfo.longitude = pChunk->geoloc().Longitude;
        tInfo.sizeLatitude = pChunk->size().Latitude;
        tInfo.sizeLongitude = pChunk->size().Longitude;
        tInfo.selfReady = pChunk->terrain() != nullptr && pChunk->terrain()->isOK();
        tInfo.childCount = pChunk->childComponents().count();
        tInfo.drawableChildren = 0;
        tInfo.stay = enoughDetail(pChunk, gCamera, iLevel);

        QSP<CWorldChunk> pNext;

        foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            if (pChild == nullptr) continue;

            if (pChild->drawable()) tInfo.drawableChildren++;

            if (pNext == nullptr && pChild->worldBounds().containsSpherical(gCamera))
            {
                pNext = pChild;
            }
        }

        m_vLastCameraPath.append(tInfo);

        pChunk = pNext;
        iLevel--;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Called by paint() to collect the chunks to draw for \a pChunk and its children. \br\br
    \a pFallbackChunk is the closest ready ancestor of \a pChunk: when neither
    the chunk nor its children can be drawn yet, it is collected instead of
    leaving a hole. Finer data always wins over this fallback on later frames.
*/
void CWorldTerrain::paintRecurse(QVector<QSP<CWorldChunk> >& vChunkCollect, CRenderContext* pContext, QSP<CWorldChunk> pChunk, int iLevel)
{
    if (pChunk == nullptr) return;

    if (skipChunkForLayerPass(pChunk, pContext)) return;

    CGeoloc gChunkPosition = pChunk->geoloc();

    int iChildCount = 0;
    int iReadyChildren = 0;

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild == nullptr) continue;

        iChildCount++;

        if (pChild->drawable())
        {
            iReadyChildren++;
        }
    }

    bool bSelfReady = pChunk->terrain() && pChunk->terrain()->isOK();

    // One zone is drawn by exactly one level: the node asks the LOD policy.
    // A self draw never descends, a hand-over never draws self: the same
    // ground is never collected twice, no fallback is ever needed.
    switch (CWorldLodPolicy::decideDraw(bSelfReady, iReadyChildren, iChildCount))
    {
    case CWorldLodPolicy::LodHandToChildren:
    {
        // Four children ready: they partition the zone, never draw self
        foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            paintRecurse(vChunkCollect, pContext, pChild, iLevel - 1);
        }
    }
    break;

    case CWorldLodPolicy::LodDrawSelf:
    {
        // Children incomplete: draw the own patch, do not descend
        pChunk->setDistance(
                    (
                        gChunkPosition.toVector3() -
                        pContext->camera()->geoloc().toVector3()
                        )
                    .magnitude()
                    );

        // Paint this chunk
        vChunkCollect.append(pChunk);

        // Get rid of unneeded water
        if (pChunk->water() != nullptr && pChunk->terrain()->allHeightsOverSea())
        {
            pChunk->setWater(QSP<CTerrain>());
        }

        // Get rid of empty nodes
        for (int iIndex = 0; iIndex < pChunk->childComponents().count(); iIndex++)
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChunk->childComponents()[iIndex]);

            if (pChild != nullptr && pChild->isEmpty())
            {
                pChunk->childComponents().remove(iIndex);
                iIndex--;
            }
        }
    }
    break;

    default:
    {
        // Nothing ready here: visit the ready children, they may cover part of the zone
        foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            if (pChild != nullptr && pChild->drawable())
            {
                paintRecurse(vChunkCollect, pContext, pChild, iLevel - 1);
            }
        }
    }
    break;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Registers ready chunk grids as physics height fields, drops stale ones
*/
void CWorldTerrain::syncPhysicsHeightfields()
{
    if (m_pRoot == nullptr || m_pScene == nullptr)
    {
        return;
    }

    if (m_pScene->physicsWorld() == nullptr)
    {
        return;
    }

    syncPhysicsHeightfieldsRecurse(m_pRoot);
}

//-------------------------------------------------------------------------------------------------

/*!
    Registers \a pChunk 's grid once its terrain is ready, drops the height
    field when the grid is gone, then visits child chunks
*/
void CWorldTerrain::syncPhysicsHeightfieldsRecurse(QSP<CWorldChunk> pChunk)
{
    if (pChunk == nullptr || m_pScene == nullptr)
    {
        return;
    }

    CPhysicsWorld* pWorld = m_pScene->physicsWorld();

    if (pWorld == nullptr)
    {
        return;
    }

    m_pScene->ensurePhysicsReference(pChunk->geoloc());

    if (chunkHeightfieldIndex(pChunk) < 0)
    {
        // Not registered yet: copy the grid once and register it
        QVector<float> vGrid;
        int iPoints = 0;

        if (pChunk->terrainGrid(vGrid, iPoints) == true)
        {
            SGeoFrame cFrame = CGeoFrame::heightfieldFrame(
                pChunk->geoloc(),
                pChunk->size(),
                iPoints,
                m_pScene->physicsReference().toVector3()
                );

            int iHeightfield = pWorld->addHeightfield(vGrid, iPoints, cFrame.SpacingXM, cFrame.SpacingZM, cFrame.Position, cFrame.Rotation);

            if (iHeightfield < 0)
            {
                LOG_METHOD_WARNING("Chunk height field registration failed");
            }

            setChunkHeightfieldIndex(pChunk, iHeightfield);
        }
        else
        {
            removeChunkHeightfield(pChunk);
        }
    }
    else if (pChunk->terrainGridReady() == false)
    {
        // Already registered: cheap liveness check only, no grid copy
        removeChunkHeightfield(pChunk);
    }

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild != nullptr)
        {
            syncPhysicsHeightfieldsRecurse(pChild);
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes the physics height field of \a pChunk when registered
*/
void CWorldTerrain::removeChunkHeightfield(QSP<CWorldChunk> pChunk)
{
    int iHeightfield = chunkHeightfieldIndex(pChunk);

    if (pChunk == nullptr || iHeightfield < 0)
    {
        return;
    }

    if (m_pScene != nullptr && m_pScene->physicsWorld() != nullptr)
    {
        m_pScene->physicsWorld()->removeHeightfield(iHeightfield);
    }

    setChunkHeightfieldIndex(pChunk, -1);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the registered height field id of \a pChunk, -1 when none
*/
int CWorldTerrain::chunkHeightfieldIndex(QSP<CWorldChunk> pChunk) const
{
    if (pChunk == nullptr)
    {
        return -1;
    }

    return m_mPhysicsHeightfields.value(pChunk.data(), -1);
}

//-------------------------------------------------------------------------------------------------

/*!
    Records \a iIndex as the height field of \a pChunk, -1 unregisters without removal
*/
void CWorldTerrain::setChunkHeightfieldIndex(QSP<CWorldChunk> pChunk, int iIndex)
{
    if (pChunk == nullptr)
    {
        return;
    }

    if (iIndex < 0)
    {
        m_mPhysicsHeightfields.remove(pChunk.data());
    }
    else
    {
        m_mPhysicsHeightfields.insert(pChunk.data(), iIndex);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes every registered chunk height field from the physics world
*/
void CWorldTerrain::clearPhysicsHeightfields()
{
    if (m_mPhysicsHeightfields.count() == 0)
    {
        return;
    }

    CPhysicsWorld* pWorld = nullptr;

    if (m_pScene != nullptr)
    {
        pWorld = m_pScene->physicsWorld();
    }

    // Note: keys are copied, removal during iteration is safe
    foreach (const CWorldChunk* pChunk, m_mPhysicsHeightfields.keys())
    {
        int iHeightfield = m_mPhysicsHeightfields.value(pChunk, -1);

        if (pWorld != nullptr && iHeightfield >= 0)
        {
            pWorld->removeHeightfield(iHeightfield);
        }
    }

    m_mPhysicsHeightfields.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Tells whether \a pChunk or any node below it is queued or running in the worker manager
*/
static bool subtreeHasWorker(QSP<CWorldChunk> pChunk)
{
    if (pChunk == nullptr) return false;

    if (CWorkerManager::getInstance()->containsWorker(pChunk.data())) return true;

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        if (subtreeHasWorker(QSP_CAST(CWorldChunk, pChildComponent))) return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys the terrain, water and height field of \a pChunk and every node below it
*/
void CWorldTerrain::destroyChunkSubtree(QSP<CWorldChunk> pChunk)
{
    if (pChunk == nullptr) return;

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        destroyChunkSubtree(QSP_CAST(CWorldChunk, pChildComponent));
    }

    removeChunkHeightfield(pChunk);

    pChunk->clearLinks(m_pScene);
    pChunk->clearTerrain();
}

//-------------------------------------------------------------------------------------------------

/*!
    Called by paint in order to collect any useless chunks.
*/
void CWorldTerrain::collectGarbage()
{
    if (m_pRoot != nullptr)
    {
        collectGarbageRecurse(m_pRoot, m_iLevels);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Applies the LOD policy bottom-up: a node covered by four ready children
    loses its patch, unused children of a covering node go away. \br\br
    \a iLevel is the depth of \a pChunk in the chunk tree.
*/
void CWorldTerrain::collectGarbageRecurse(QSP<CWorldChunk> pChunk, int iLevel)
{
    if (pChunk == nullptr) return;

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild != nullptr)
        {
            collectGarbageRecurse(pChild, iLevel - 1);
        }
    }

    int iChildCount = 0;
    int iReadyChildren = 0;
    bool bChildrenGoingAway = pChunk->childComponents().count() > 0;

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild == nullptr) continue;

        iChildCount++;

        if (pChild->drawable())
        {
            iReadyChildren++;
        }

        // Going away means nobody needs the zone: out of range, or dead weight.
        // Workers in flight keep their subtree, and needed children stay.
        bool bChildNeeded = CWorldLodPolicy::isNeeded(pChild->originalGeoloc(), pChild->originalSize(), m_gLastCameraGeoloc);

        if ((bChildNeeded == false || pChild->isEmpty()) == false || subtreeHasWorker(pChild))
        {
            bChildrenGoingAway = false;
        }
    }

    bool bAllChildrenReady = CWorldLodPolicy::allChildrenReady(iReadyChildren, iChildCount);
    bool bCovered = bAllChildrenReady || iChildCount == 0;
    bool bNeeded = CWorldLodPolicy::isNeeded(pChunk->originalGeoloc(), pChunk->originalSize(), m_gLastCameraGeoloc);

    // R2: covered and out of range, the own patch may go unless protected.
    // Only the patch is freed, the tree links stay. No timer: needed is the
    // policy visibility, useless patches go on the next garbage pass.
    if (pChunk->terrain() != nullptr
        && CWorldLodPolicy::mayDestroySelf(bCovered, iLevel, m_iLevels, bNeeded))
    {
        removeChunkHeightfield(pChunk);

        pChunk->clearTerrain();
    }

    bool bSelfReady = pChunk->terrain() != nullptr && pChunk->terrain()->isOK();

    // R3: unneeded children go away, the parent covers them
    if (bAllChildrenReady == false && bChildrenGoingAway)
    {
        if (bSelfReady == false && bNeeded)
        {
            // Parent cover missing but needed: rebuild it, prune the children on a later pass
            ensureChunkTerrain(pChunk, iLevel);

            requestChunkDetailWork(pChunk);
        }
        else
        {
            // Parent covers, or nobody needs the zone: prune
            for (int iIndex = 0; iIndex < pChunk->childComponents().count(); iIndex++)
            {
                QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChunk->childComponents()[iIndex]);

                destroyChunkSubtree(pChild);

                pChunk->childComponents().remove(iIndex);
                iIndex--;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the height at the specified \a gPosition. \br\br
    \a pRigidness, if not nullptr, is filled with the terrain rigidness at the specified location.
*/
double CWorldTerrain::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (m_pRoot != nullptr)
    {
        return getHeightAtRecurse(gPosition, m_pRoot, pRigidness);
    }

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

double CWorldTerrain::getHeightAtRecurse(const CGeoloc& gPosition, QSP<CWorldChunk> pChunk, double* pRigidness)
{
    double dDiffLatitude = Math::Angles::angleDifferenceDegree(gPosition.Latitude, pChunk->geoloc().Latitude);
    double dDiffLongitude = Math::Angles::angleDifferenceDegree(gPosition.Longitude, pChunk->geoloc().Longitude);

    if (pRigidness != nullptr) *pRigidness = 0.0;

    if (
            fabs(dDiffLatitude) < pChunk->size().Latitude * 0.5 &&
            fabs(dDiffLongitude) < pChunk->size().Longitude * 0.5
            )
    {
        foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            double dNewRigidness = 0.0;
            double dNewAltitude = getHeightAtRecurse(gPosition, pChild, &dNewRigidness);

            if (dNewAltitude != Q3D_INFINITY)
            {
                if (pRigidness != nullptr) *pRigidness = dNewRigidness;
                return dNewAltitude;
            }
        }

        if (pChunk->terrain() && pChunk->terrain()->isOK())
        {
            double dTerrainRigidness = 0.0;
            double dTerrainAltitude = pChunk->terrain()->getHeightAt(gPosition, &dTerrainRigidness);

            if (pChunk->water())
            {
                double dWaterRigidness = 0.0;
                double dWaterAltitude = pChunk->water()->getHeightAt(gPosition, &dWaterRigidness);

                if (dWaterAltitude != Q3D_INFINITY && dWaterAltitude > dTerrainAltitude)
                {
                    dTerrainRigidness = dWaterRigidness;
                    dTerrainAltitude = dWaterAltitude;
                }
            }

            if (dTerrainAltitude != Q3D_INFINITY)
            {
                if (pRigidness != nullptr) *pRigidness = dTerrainRigidness;
                return dTerrainAltitude;
            }

        }
    }

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

/*!
    Flattens terrain at the specified \a gPosition, to the extents of \a dRadius_m in meters.
*/
void CWorldTerrain::flatten(const CGeoloc& gPosition, double dRadius_m)
{
    if (m_pRoot != nullptr)
    {
        return flattenRecurse(m_pRoot, gPosition, dRadius_m);
    }
}

//-------------------------------------------------------------------------------------------------

void CWorldTerrain::flattenRecurse(QSP<CWorldChunk> pChunk, const CGeoloc& gPosition, double dRadius_m)
{
    if (pChunk->terrain() != nullptr)
    {
    }

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild != nullptr)
        {
            flattenRecurse(pChild, gPosition, dRadius_m);
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a ray intersects this component.
*/
RayTracingResult CWorldTerrain::intersect(Math::CRay3 ray)
{
    if (m_pRoot != nullptr)
    {
        return intersectRecurse(m_pRoot, ray);
    }

    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a ray intersects this terrain, using \a pChunk as current chunk.
*/
RayTracingResult CWorldTerrain::intersectRecurse(QSP<CWorldChunk> pChunk, const Math::CRay3& ray) const
{
    RayTracingResult dResult(Q3D_INFINITY);

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild != nullptr)
        {
            RayTracingResult dNewResult = intersectRecurse(pChild, ray);

            if (dNewResult.m_dDistance < dResult.m_dDistance)
            {
                dResult = dNewResult;
            }
        }
    }

    RayTracingResult dNewResult = pChunk->intersect(ray);

    if (dNewResult.m_dDistance < dResult.m_dDistance)
    {
        dResult = dNewResult;
    }

    return dResult;
}

//-------------------------------------------------------------------------------------------------

/*!
    Dumps this component to \a stream using the indentation value in \a iIdent.
*/
void CWorldTerrain::dump(QTextStream& stream, int iIdent)
{
    dumpIndented(stream, iIdent, QString("[CWorldTerrain]"));
    dumpIndented(stream, iIdent, QString("GenerateNow : %1").arg(m_bGenerateNow));
    dumpIndented(stream, iIdent, QString("Levels : %1").arg(m_iLevels));
    dumpIndented(stream, iIdent, QString("Terrain resolution : %1").arg(m_iTerrainResolution));
    dumpIndented(stream, iIdent, QString("Root :"));

    if (m_pRoot != nullptr)
    {
        dumpOpenBlock(stream, iIdent); iIdent++;
        m_pRoot->dump(stream, iIdent);
        iIdent--; dumpCloseBlock(stream, iIdent);
    }

    CComponent::dump(stream, iIdent);
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns a JSON document with the last collect counts and the quadtree readiness on the camera path.
*/
QString CWorldTerrain::snapshotJson()
{
    QString sCollect = "[";

    bool bFirstCollect = true;

    foreach (int iLayer, m_mLastCollectedChunks.keys())
    {
        if (bFirstCollect == false) sCollect += ",";
        bFirstCollect = false;

        sCollect += QString("{\"layer\":%1,\"overlaps\":%2,\"chunks\":[").arg(iLayer).arg(m_mLastOverlapCount.value(iLayer, 0));

        bool bFirstChunk = true;

        foreach (const SCollectedChunkInfo& tChunk, m_mLastCollectedChunks[iLayer])
        {
            if (bFirstChunk == false) sCollect += ",";
            bFirstChunk = false;

            sCollect += tChunk.snapshotJson();
        }

        sCollect += "]}";
    }

    sCollect += "]";

    QString sPath = "[";

    bool bFirstLevel = true;

    foreach (const SChunkPathInfo& tLevel, m_vLastCameraPath)
    {
        if (bFirstLevel == false) sPath += ",";
        bFirstLevel = false;

        sPath += tLevel.snapshotJson();
    }

    sPath += "]";

    return QString("{%1,\"collect\":%2,\"path\":%3}")
        .arg(snapshotBaseJson())
        .arg(sCollect)
        .arg(sPath);
}
