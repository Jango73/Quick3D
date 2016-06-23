
// Application
#include "CLogManager.h"
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CAutoTerrain.h"
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
    \class CAutoTerrain
    \brief A dynamic terrain, with automatic LOD.
    \inmodule Quick3D
    \sa C3DScene
*/

//-------------------------------------------------------------------------------------------------

/*!
    Instantiates a new CAutoTerrain.
*/
CComponent* CAutoTerrain::instanciator(C3DScene* pScene)
{
    return new CAutoTerrain(pScene);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CAutoTerrain \br\br.
    \a pScene is the scene to which this terrain belongs.
    \a gCameraPosition is the geo-localization of the camera, used to build the initial terrain patches.
    \a pHeights is the height field to use for altitudes.
    \a bGenerateNow tells whether the terrain should be immediately created or threaded.
*/
CAutoTerrain::CAutoTerrain(C3DScene* pScene, CGeoloc gCameraPosition, CHeightField* pHeights, bool bGenerateNow)
    : CComponent(pScene)
    , m_bGenerateNow(bGenerateNow)
    , m_pRoot(NULL)
    , m_pHeights(pHeights)
    , m_pMaterial(NULL)
    , m_iLevels(15)
    , m_iTerrainResolution(31)
{
    setName("AutoTerrain");
    setInheritTransform(false);

    m_pMaterial = pScene->getRessourcesManager()->shareMaterial(QSP<CMaterial>(new CMaterial(m_pScene)));

    if (m_bGenerateNow)
    {
        LOG_DEBUG("CAutoTerrain::CAutoTerrain() : Generating terrain now");

        if (pScene->getViewports().count() > 0 && pScene->getViewports()[0]->getCamera())
        {
            buildRoot();

            CRenderContext context(
                        QMatrix4x4(),
                        QMatrix4x4(),
                        QMatrix4x4(),
                        QMatrix4x4(),
                        Math::CMatrix4(),
                        Math::CMatrix4(),
                        pScene,
                        pScene->getViewports()[0]->getCamera().data()
                    );

            buildRecurse(m_pRoot, &context, m_iLevels);
        }
        else
        {
            LOG_WARNING("CAutoTerrain::CAutoTerrain() : Scene has no viewport");
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CAutoTerrain.
*/
CAutoTerrain::~CAutoTerrain()
{
    foreach (CVegetation* pVegetation, m_vVegetation)
    {
        delete pVegetation;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets the terrain resolution to \a value.
*/
void CAutoTerrain::setTerrainResolution(int value)
{
    m_iTerrainResolution = Angles::clipInt(value, 3, 81);
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this terrain from \a xComponent.
*/
void CAutoTerrain::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
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

    readVegetationParameters(sBaseFile, xFunctionsNode);
    readBuildingParameters(sBaseFile, xFunctionsNode);

    CXMLNode xMaterialNode = m_xParameters.getNodeByTagName(ParamName_Material);

    QString sMaterialType = xMaterialNode.attributes()[ParamName_Type];

    if (sMaterialType.toLower() == "blend")
    {
        m_pMaterial = m_pScene->getRessourcesManager()->shareMaterial(QSP<CMaterial>(new CMaterial(m_pScene)));

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

        m_pMaterial = m_pScene->getRessourcesManager()->shareMaterial(QSP<CMaterial>(pTiled));
    }
}

//-------------------------------------------------------------------------------------------------

void CAutoTerrain::clearLinks(C3DScene* pScene)
{
    if (m_pRoot)
    {
        m_pRoot->clearLinks(pScene);
    }

    m_pRoot.reset();
}

//-------------------------------------------------------------------------------------------------

/*!
    Paints the terrain using \a pContext.
*/
void CAutoTerrain::paint(CRenderContext* pContext)
{
    static int iBuildCounter = 0;
    static int iGarbageCounter = 0;

    buildRoot();

    iBuildCounter++;

    if (iBuildCounter > 5)
    {
        iBuildCounter = 0;
        buildRecurse(m_pRoot, pContext, m_iLevels);
    }

    QVector<QSP<CWorldChunk> > vChunkCollect;

    paintRecurse(vChunkCollect, pContext, m_pRoot, m_iLevels, false);

    qSort(vChunkCollect.begin(), vChunkCollect.end());

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
void CAutoTerrain::update(double dDeltaTime)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Does post update work on the terrain using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CAutoTerrain::postUpdate(double dDeltaTime)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Builds the root of the terrain.
*/
void CAutoTerrain::buildRoot()
{
    if (!m_pRoot)
    {
        m_pRoot = QSP<CWorldChunk>(new CWorldChunk(m_pScene, this, this));
        m_pRoot->setInheritTransform(false);
        m_pRoot->CComponent::setParent(QSP<CComponent>(this));
        m_pRoot->setGeoloc(CGeoloc(0.0, 180.0, 0.0));
        m_pRoot->computeWorldTransform();
        m_pRoot->setSize(CGeoloc(LAT_MAX * 2.0, 360.0, 0.0));
    }
}

//-------------------------------------------------------------------------------------------------

bool CAutoTerrain::enoughDetail(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel)
{
    // double dDistance = (pContext->camera()->getWorldPosition() - pChunk->getWorldBounds().center()).getMagnitude();
    // return pChunk->getWorldBounds().radius() / dDistance < 2.0;

    // if (iLevel > m_iLevels - (m_iLevels / 5)) return false;

    return (
                pChunk->getWorldBounds().containsSpherical(pContext->camera()->getGeoloc()) == false ||
                iLevel == 0
                )
            && iLevel < m_iLevels;
}

//-------------------------------------------------------------------------------------------------

/*!
    Called by paint() to build necessary terrain patches given the camera location. \br\br
    \a pChunk is any chunk in the chunk tree of the terrain. \br
    \a pContext is the rendering context.
    \a iLevel is the depth in the chunk tree.
*/
void CAutoTerrain::buildRecurse(QSP<CWorldChunk> pChunk, CRenderContext* pContext, int iLevel)
{
    CGeoloc gOriginalChunkPosition = pChunk->getOriginalGeoloc();
    CGeoloc gOriginalChunkSize = pChunk->getOriginalSize();

    CGeoloc gChunkPosition = pChunk->getGeoloc();
    CGeoloc gChunkSize = pChunk->getSize();

    // On décide si ce niveau de détail est suffisant
    // Decide whether this level of detail is enough
    bool bStayHere = enoughDetail(pChunk, pContext, iLevel);

    if (bStayHere)
    {
        // Create terrain for the chunk if needed
        if (pChunk->getTerrain() == NULL)
        {
            LOG_DEBUG(QString("Creating terrain for tile at lat %1, lon %2, level %3")
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

                if (pTiled == NULL)
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
    else
    {
        // Création des sous-chunks si besoin
        if (pChunk->getChildren().count() == 0)
        {
            LOG_DEBUG(QString("Creating sub-quads for tile at lat %1, lon %2, level %3")
                      .arg(gChunkPosition.Latitude)
                      .arg(gChunkPosition.Longitude)
                      .arg(iLevel)
                      );

            CGeoloc gStart(
                        gOriginalChunkPosition.Latitude - gOriginalChunkSize.Latitude * 0.5,
                        gOriginalChunkPosition.Longitude - gOriginalChunkSize.Longitude * 0.5,
                        0.0
                        );

            CGeoloc gSize(
                        gOriginalChunkSize.Latitude * 0.5,
                        gOriginalChunkSize.Longitude * 0.5,
                        0.0
                        );

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
            pChunk->getChildren().append(pChild1);
            pChunk->getChildren().append(pChild2);
            pChunk->getChildren().append(pChild3);
            pChunk->getChildren().append(pChild4);

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

        // Recurse in child chunks
        foreach (QSP<CComponent> pChildComponent, pChunk->getChildren())
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            buildRecurse(pChild, pContext, iLevel - 1);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CAutoTerrain::paintRecurse(QVector<QSP<CWorldChunk> >& vChunkCollect, CRenderContext* pContext, QSP<CWorldChunk> pChunk, int iLevel, bool bForcePaint)
{
    CGeoloc gChunkPosition = pChunk->getGeoloc();

    // On décide si ce niveau de détail est suffisant
    // Decide whether this level of detail is enough
    bool bStayHere = enoughDetail(pChunk, pContext, iLevel);

    bool bChildrenDrawable = true;

    foreach (QSP<CComponent> pChildComponent, pChunk->getChildren())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild->drawable() == false)
        {
            bChildrenDrawable = false;
            break;
        }
    }

    if (bStayHere)
    {
        if (pChunk->getTerrain() != NULL && pChunk->getTerrain()->isOK())
        {
            pChunk->setDistance(
                        (
                            gChunkPosition.toVector3() -
                            pContext->camera()->getGeoloc().toVector3()
                            )
                        .getMagnitude()
                        );

            // Paint this chunk
            vChunkCollect.append(pChunk);

            // Get rid of unneeded water
            if (pChunk->getWater() && pChunk->getTerrain()->getAllHeightsOverSea())
            {
                pChunk->setWater(QSP<CTerrain>());
            }

            // Get rid of empty nodes
            for (int iIndex = 0; iIndex < pChunk->getChildren().count(); iIndex++)
            {
                QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChunk->getChildren()[iIndex]);

                if (pChild && pChild->isEmpty())
                {
                    pChunk->getChildren().remove(iIndex);
                    iIndex--;
                }
            }
        }
        else if (bChildrenDrawable)
        {
            // Paint children
            foreach (QSP<CComponent> pChildComponent, pChunk->getChildren())
            {
                QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

                paintRecurse(vChunkCollect, pContext, pChild, iLevel - 1, false);
            }
        }
    }
    else
    {
        if (bChildrenDrawable)
        {
            // Paint children
            foreach (QSP<CComponent> pChildComponent, pChunk->getChildren())
            {
                QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

                paintRecurse(vChunkCollect, pContext, pChild, iLevel - 1, false);
            }
        }
        else
        {
            pChunk->setDistance(
                        (
                            gChunkPosition.toVector3() -
                            pContext->camera()->getGeoloc().toVector3()
                            )
                        .getMagnitude()
                        );

            // Paint this chunk
            vChunkCollect.append(pChunk);
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Called by paint in order to collect any useless chunks.
*/
void CAutoTerrain::collectGarbage()
{
    if (m_pRoot)
    {
        collectGarbageRecurse(m_pRoot);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys \a pChunk 's terrain if it has not been used since 60 seconds and collects garbage on child chunks.
*/
void CAutoTerrain::collectGarbageRecurse(QSP<CWorldChunk> pChunk)
{
    if (pChunk->getTerrain() != NULL)
    {
        if (pChunk->isExpendable())
        {
            pChunk->clearTerrain();
        }
    }

    foreach (QSP<CComponent> pChildComponent, pChunk->getChildren())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild)
        {
            collectGarbageRecurse(pChild);
        }
    }
}

//-------------------------------------------------------------------------------------------------

double CAutoTerrain::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (m_pRoot)
    {
        return getHeightAtRecurse(gPosition, m_pRoot, pRigidness);
    }

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

double CAutoTerrain::getHeightAtRecurse(const CGeoloc& gPosition, QSP<CWorldChunk> pChunk, double* pRigidness)
{
    double dDiffLatitude = Math::Angles::angleDifferenceDegree(gPosition.Latitude, pChunk->getGeoloc().Latitude);
    double dDiffLongitude = Math::Angles::angleDifferenceDegree(gPosition.Longitude, pChunk->getGeoloc().Longitude);

    if (pRigidness) *pRigidness = 0.0;

    if (
            fabs(dDiffLatitude) < pChunk->getSize().Latitude * 0.5 &&
            fabs(dDiffLongitude) < pChunk->getSize().Longitude * 0.5
            )
    {
        foreach (QSP<CComponent> pChildComponent, pChunk->getChildren())
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            double dNewRigidness = 0.0;
            double dNewAltitude = getHeightAtRecurse(gPosition, pChild, &dNewRigidness);

            if (dNewAltitude != Q3D_INFINITY)
            {
                if (pRigidness) *pRigidness = dNewRigidness;
                return dNewAltitude;
            }
        }

        if (pChunk->getTerrain() != NULL && pChunk->getTerrain()->isOK())
        {
            double dTerrainRigidness = 0.0;
            double dTerrainAltitude = pChunk->getTerrain()->getHeightAt(gPosition, &dTerrainRigidness);

            if (pChunk->getWater() != NULL)
            {
                double dWaterRigidness = 0.0;
                double dWaterAltitude = pChunk->getWater()->getHeightAt(gPosition, &dWaterRigidness);

                if (dWaterAltitude != Q3D_INFINITY && dWaterAltitude > dTerrainAltitude)
                {
                    dTerrainRigidness = dWaterRigidness;
                    dTerrainAltitude = dWaterAltitude;
                }
            }

            if (dTerrainAltitude != Q3D_INFINITY)
            {
                if (pRigidness) *pRigidness = dTerrainRigidness;
                return dTerrainAltitude;
            }

        }
    }

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

/*!
    Flattens terrain at the specified \a gPosition, to the extents of \a dRadius.
*/
void CAutoTerrain::flatten(const CGeoloc& gPosition, double dRadius)
{
    /*
    foreach (ChunkMap map, m_vChunks)
    {
        if (map.pChunk)
        {
            if (map.pChunk->getWorldBounds().contains(gPosition, dRadius))
            {
                map.pChunk->flatten(gPosition, dRadius);
            }
        }
    }
    */
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads parameters for vegetation and creates appropriate meshes.
*/
void CAutoTerrain::readVegetationParameters(const QString& sBaseFile, CXMLNode xFunctions)
{
    CXMLNode xVegetationNode = m_xParameters.getNodeByTagName(ParamName_Vegetation);

    QVector<CXMLNode> xTrees = xVegetationNode.getNodesByTagName(ParamName_Tree);

    foreach (CXMLNode xTree, xTrees)
    {
        CXMLNode xGeneral = xTree.getNodeByTagName(ParamName_General);
        CXMLNode xDNA = xTree.getNodeByTagName(ParamName_DNA);
        CXMLNode xCoverage = xTree.getNodeByTagName(ParamName_Coverage);

        CGenerateFunction* pFunction = new CGenerateFunction(xFunctions, xCoverage.getNodeByTagName(ParamName_Value));

        double dSpread = xGeneral.attributes()[ParamName_Spread].toDouble();
        int iLevels = xDNA.attributes()[ParamName_Levels].toDouble();
        double dTrunkLength = xDNA.attributes()[ParamName_TrunkLength].toDouble();
        double dTrunkRadius = xDNA.attributes()[ParamName_TrunkRadius].toDouble();
        double dBranchLengthScale = xDNA.attributes()[ParamName_BranchLengthScale].toDouble();
        double dBranchRadiusScale = xDNA.attributes()[ParamName_BranchRadiusScale].toDouble();
        double dLeafScale = xDNA.attributes()[ParamName_LeafScale].toDouble();
        double dGravityFactor = xDNA.attributes()[ParamName_GravityFactor].toDouble();

        CXMLNode xLeaf = xDNA.getNodeByTagName(ParamName_Leaf);
        CXMLNode xFFD = xLeaf.getNodeByTagName(ParamName_FFD);

        QVector<CXMLNode> xPoints = xFFD.getNodesByTagName(ParamName_Point);

        QVector<CVector3> vFFDFrom;
        QVector<CVector3> vFFDTo;

        foreach (CXMLNode xPoint, xPoints)
        {
            vFFDFrom.append(CVector3(
                                xPoint.attributes()["fx"].toDouble(),
                            xPoint.attributes()["fy"].toDouble(),
                    xPoint.attributes()["fz"].toDouble()
                    ));

            vFFDTo.append(CVector3(
                              xPoint.attributes()["tx"].toDouble(),
                          xPoint.attributes()["ty"].toDouble(),
                    xPoint.attributes()["tz"].toDouble()
                    ));
        }

        QVector<QSP<CMesh> > vMeshes;

        CVector3 vNoisePosition(
                    ((double) rand() / 32768.0) * 2.0,
                    ((double) rand() / 32768.0) * 2.0,
                    ((double) rand() / 32768.0) * 2.0
                    );

        for (int iLODLevel = 0; iLODLevel < 5; iLODLevel++)
        {
            CMesh* pMesh = new CMesh(m_pScene);

            CMeshGeometry* pMeshGeometry = m_pScene->getTreeGenerator()->createTree(
                        iLODLevel,
                        vNoisePosition,
                        iLevels,
                        dTrunkLength,
                        dTrunkRadius,
                        dBranchLengthScale,
                        dBranchRadiusScale,
                        dLeafScale,
                        dGravityFactor,
                        vFFDFrom,
                        vFFDTo
                        );

            pMesh->setGeometry(QSP<CMeshGeometry>(pMeshGeometry));
            vMeshes.append(QSP<CMesh>(pMesh));
        }

        m_vVegetation.append(new CVegetation(CVegetation::evtTree, dSpread, pFunction, new CMeshInstance(vMeshes), NULL));
    }

    QVector<CXMLNode> xBushes = xVegetationNode.getNodesByTagName(ParamName_Bush);

    foreach (CXMLNode xBush, xBushes)
    {
        CXMLNode xGeneral = xBush.getNodeByTagName(ParamName_General);
        CXMLNode xCoverage = xBush.getNodeByTagName(ParamName_Coverage);
        CXMLNode xMaterial = xBush.getNodeByTagName(ParamName_Material);

        if (xCoverage.isEmpty() == false && xGeneral.attributes()[ParamName_Spread].isEmpty() == false)
        {
            CGenerateFunction* pFunction = new CGenerateFunction(xFunctions, xCoverage.getNodeByTagName(ParamName_Value));

            double dSpread = xGeneral.attributes()[ParamName_Spread].toDouble();

            if (xMaterial.isEmpty() == false)
            {
                CMaterial* pMaterial = new CMaterial(m_pScene);
                pMaterial->loadParameters(sBaseFile, xMaterial);
                pMaterial->setBillBoard(true);
                pMaterial->setHasAlpha(true);
                m_vVegetation.append(new CVegetation(CVegetation::evtBush, dSpread, pFunction, NULL, pMaterial));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Reads parameters for buildings.
*/
void CAutoTerrain::readBuildingParameters(const QString& sBaseFile, CXMLNode xFunctions)
{
    Q_UNUSED(sBaseFile);

    CXMLNode xBuildingsNode = m_xParameters.getNodeByTagName(ParamName_Buildings);

    QVector<CXMLNode> xBuildings = xBuildingsNode.getNodesByTagName(ParamName_Building);

    foreach (CXMLNode xBuilding, xBuildings)
    {
        CXMLNode xMeshList = xBuilding.getNodeByTagName(ParamName_MeshList);
        CXMLNode xCoverage = xBuilding.getNodeByTagName(ParamName_Coverage);

        // TODO : Complete
    }
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CAutoTerrain::intersect(Math::CRay3 ray)
{
    if (m_pRoot)
    {
        return intersectRecurse(m_pRoot, ray);
    }

    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

/*!
    Checks if \a ray intersects this terrain.
*/
RayTracingResult CAutoTerrain::intersectRecurse(QSP<CWorldChunk> pChunk, const Math::CRay3& ray) const
{
    RayTracingResult dResult(Q3D_INFINITY);

    foreach (QSP<CComponent> pChildComponent, pChunk->getChildren())
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild)
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
void CAutoTerrain::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CAutoTerrain]"));
    dumpIdent(stream, iIdent, QString("Levels : %1").arg(m_iLevels));
    dumpIdent(stream, iIdent, QString("Terrain res : %1").arg(m_iTerrainResolution));
    dumpIdent(stream, iIdent, QString("Root :"));

    if (m_pRoot)
    {
        dumpOpenBlock(stream, iIdent); iIdent++;
        m_pRoot->dump(stream, iIdent);
        iIdent--; dumpCloseBlock(stream, iIdent);
    }

    CComponent::dump(stream, iIdent);
}
