
// qt-plus
#include "CLogger.h"
#include "CTimeSampler.h"

// Std
#include <algorithm>

// Qt
#include <QMutexLocker>

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

    {
        CScopedTimeSample tSample(QString("Terrain_Collect"));
        paintRecurse(vChunkCollect, pContext, m_pRoot, m_iLevels);
    }

    int iPaintLayer = (pContext != nullptr && pContext->camera() != nullptr) ? pContext->camera()->currentPaintLayer() : -1;

    QVector<COLLECTED_CHUNK_INFO> vCollected;

    foreach (QSP<CWorldChunk> pCollected, vChunkCollect)
    {
        if (pCollected == nullptr) continue;

        COLLECTED_CHUNK_INFO tInfo;
        tInfo.m_sAddress = QString::number(quintptr(pCollected.data()), 16);
        tInfo.m_dLatitude = pCollected->geoloc().Latitude;
        tInfo.m_dLongitude = pCollected->geoloc().Longitude;
        tInfo.m_dSizeLatitude = pCollected->size().Latitude;
        tInfo.m_dSizeLongitude = pCollected->size().Longitude;
        vCollected.append(tInfo);
    }

    CGeoloc gCameraGeoloc;

    if (pContext != nullptr && pContext->camera() != nullptr)
    {
        gCameraGeoloc = pContext->camera()->geoloc();
    }

    {
        QMutexLocker tLocker(&m_mSnapshotMutex);
        m_mLastCollectedChunks[iPaintLayer] = vCollected;
        m_gLastCameraGeoloc = gCameraGeoloc;
    }

    if (pContext != nullptr && pContext->camera() != nullptr)
    {
        recordCameraPath(gCameraGeoloc);
    }

    std::sort(vChunkCollect.begin(), vChunkCollect.end());

    {
        CScopedTimeSample tSample(QString("Terrain_DrawLoop"));

        // Opaque phase first: consecutive chunks share the terrain material,
        // so the material activation guard hits instead of re-applying the
        // full state on every chunk
        foreach (QSP<CWorldChunk> pChunk, vChunkCollect)
        {
            pContext->tStatistics.m_iNumChunksDrawn++;

            pChunk->paintOpaquePhase(pContext);
        }

        // Transparent phase last: water over settled opaque depths
        foreach (QSP<CWorldChunk> pChunk, vChunkCollect)
        {
            pChunk->paintWaterPhase(pContext);
        }
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

    SChildStatistics tStats = pChunk->childStatistics();

    bool bSelfReady = pChunk->terrain() && pChunk->terrain()->isOK();

    // One zone is drawn by exactly one level: the node asks the LOD policy.
    // A self draw never descends, a hand-over never draws self: the same
    // ground is never collected twice, no fallback is ever needed.
    switch (CWorldLodPolicy::decideDraw(bSelfReady, tStats.drawableChildren, tStats.childCount))
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
