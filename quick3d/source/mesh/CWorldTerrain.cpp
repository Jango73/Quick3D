
// qt-plus
#include "CLogger.h"

// Std
#include <algorithm>

// Application
#include "C3DScene.h"
#include "CPhysicsMath.h"
#include "CComponentFactory.h"
#include "CWorkerManager.h"
#include "CWorldTerrain.h"
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

    paintRecurse(vChunkCollect, pContext, m_pRoot, m_iLevels, QSP<CWorldChunk>());

    std::sort(vChunkCollect.begin(), vChunkCollect.end());

    foreach (QSP<CWorldChunk> pChunk, vChunkCollect)
    {
        pContext->tStatistics.m_iNumChunksDrawn++;

        pChunk->paint(pContext);
    }

    // Note: main thread only, height field bodies are never added or removed
    // during a physics step, which runs from updateScene()
    syncPhysicsHeightfields();

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
    // double dDistance = (pContext->camera()->getWorldPosition() - pChunk->getWorldBounds().center()).getMagnitude();
    // return pChunk->getWorldBounds().radius() / dDistance < 2.0;

    // if (iLevel > m_iLevels - (m_iLevels / 5)) return false;

    return (
                pChunk->worldBounds().containsSpherical(pContext->camera()->geoloc()) == false ||
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
    CGeoloc gChunkSize = pChunk->size();

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

    if (bStayHere)
    {
        // Create terrain for the chunk if needed
        if (pChunk->terrain() == nullptr)
        {
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

        requestChunkDetailWork(pChunk);
    }
    else
    {
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

/*!
    Collects \a pFallbackChunk for drawing when nothing finer is available. \br\br
    \a pFallbackChunk is the closest ready ancestor: drawing it covers the
    area with coarser data instead of leaving a hole. It is skipped when null
    or already collected from a sibling subtree.
*/
void CWorldTerrain::collectFallbackChunk(QVector<QSP<CWorldChunk> >& vChunkCollect, CRenderContext* pContext, QSP<CWorldChunk> pFallbackChunk)
{
    if (pFallbackChunk == nullptr) return;

    pFallbackChunk->setDistance(
                (
                    pFallbackChunk->geoloc().toVector3() -
                    pContext->camera()->geoloc().toVector3()
                    )
                .magnitude()
                );

    if (vChunkCollect.contains(pFallbackChunk)) return;

    vChunkCollect.append(pFallbackChunk);
}

//-------------------------------------------------------------------------------------------------

/*!
    Called by paint() to collect the chunks to draw for \a pChunk and its children. \br\br
    \a pFallbackChunk is the closest ready ancestor of \a pChunk: when neither
    the chunk nor its children can be drawn yet, it is collected instead of
    leaving a hole. Finer data always wins over this fallback on later frames.
*/
void CWorldTerrain::paintRecurse(QVector<QSP<CWorldChunk> >& vChunkCollect, CRenderContext* pContext, QSP<CWorldChunk> pChunk, int iLevel, QSP<CWorldChunk> pFallbackChunk)
{
    if (pChunk == nullptr) return;

    CGeoloc gChunkPosition = pChunk->geoloc();

    bool bStayHere = enoughDetail(pChunk, pContext, iLevel);
    bool bChildrenDrawable = true;

    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild == nullptr || pChild->drawable() == false)
        {
            bChildrenDrawable = false;
            break;
        }
    }

    bool bSelfReady = pChunk->terrain() && pChunk->terrain()->isOK();

    QSP<CWorldChunk> pReadyAncestor = pFallbackChunk;

    if (bSelfReady)
    {
        pReadyAncestor = pChunk;
    }

    if (bStayHere)
    {
        if (bSelfReady)
        {
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
        else if (bChildrenDrawable)
        {
            // Paint children
            foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
            {
                QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

                paintRecurse(vChunkCollect, pContext, pChild, iLevel - 1, pReadyAncestor);
            }
        }
        else
        {
            // Neither this patch nor the children are ready: draw the closest
            // ready ancestor instead of leaving a hole
            collectFallbackChunk(vChunkCollect, pContext, pReadyAncestor);
        }
    }
    else
    {
        if (bChildrenDrawable)
        {
            // Paint children
            foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
            {
                QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

                paintRecurse(vChunkCollect, pContext, pChild, iLevel - 1, pReadyAncestor);
            }
        }
        else if (bSelfReady)
        {
            pChunk->setDistance(
                        (
                            gChunkPosition.toVector3() -
                            pContext->camera()->geoloc().toVector3()
                            )
                        .magnitude()
                        );

            // Paint this chunk
            vChunkCollect.append(pChunk);
        }
        else
        {
            // Children wanted but not ready, and this patch is not ready
            // either: draw the closest ready ancestor instead of a hole
            collectFallbackChunk(vChunkCollect, pContext, pReadyAncestor);
        }
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

    QVector<float> vGrid;
    int iPoints = 0;

    if (pChunk->terrainGrid(vGrid, iPoints) == true)
    {
        if (pChunk->physicsHeightfieldIndex() < 0)
        {
            CPhysicsMath::SHeightfieldFrame cFrame = CPhysicsMath::heightfieldFrame(
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

            pChunk->setPhysicsHeightfieldIndex(iHeightfield);
        }
    }
    else
    {
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
    if (pChunk == nullptr || pChunk->physicsHeightfieldIndex() < 0)
    {
        return;
    }

    if (m_pScene != nullptr && m_pScene->physicsWorld() != nullptr)
    {
        m_pScene->physicsWorld()->removeHeightfield(pChunk->physicsHeightfieldIndex());
    }

    pChunk->setPhysicsHeightfieldIndex(-1);
}

//-------------------------------------------------------------------------------------------------

/*!
    Called by paint in order to collect any useless chunks.
*/
void CWorldTerrain::collectGarbage()
{
    if (m_pRoot != nullptr)
    {
        collectGarbageRecurse(m_pRoot);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys \a pChunk 's terrain if it has not been used since 60 seconds and collects garbage on child chunks.
*/
void CWorldTerrain::collectGarbageRecurse(QSP<CWorldChunk> pChunk)
{
    foreach (QSP<CComponent> pChildComponent, pChunk->childComponents())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild != nullptr)
        {
            collectGarbageRecurse(pChild);
        }
    }

    if (pChunk->terrain() != nullptr)
    {
        if (pChunk->isExpendable())
        {
            removeChunkHeightfield(pChunk);

            pChunk->clearLinks(m_pScene);
            pChunk->clearTerrain();
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
