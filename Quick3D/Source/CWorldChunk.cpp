
// qt-plus
#include "CLogger.h"

// Application
#include "CWorkerManager.h"
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CWorldTerrain.h"
#include "CWorldChunk.h"
#include "CTiledMaterial.h"

using namespace Math;

#define NUM_CONTAINERS	8

//-------------------------------------------------------------------------------------------------

CWorldChunk::CWorldChunk(C3DScene* pScene, CWorldTerrain* pAutoTerrain, CHeightField* pContainer)
    : CComponent(pScene)
    , m_mMutex(QMutex::Recursive)
    , m_pAutoTerrain(pAutoTerrain)
    , m_pTerrain(nullptr)
    , m_pWater(nullptr)
    , m_pContainer(pContainer)
    , m_dDistance(0.0)
    , m_bOK(false)
{
    CComponent::incComponentCounter(ClassName_CWorldChunk);

    setUsedNow();
}

//-------------------------------------------------------------------------------------------------

CWorldChunk::~CWorldChunk()
{
    CComponent::decComponentCounter(ClassName_CWorldChunk);

    // Remove this from workers
    CWorkerManager::getInstance()->removeWorker(this);

    {
        QMutexLocker locker(&m_mMutex);

        LOG_METHOD_DEBUG(QString("Deleting %1 CWorldChunk::CBoundedMeshInstances for tile at lat %2, lon %3")
                  .arg(m_vBoundedMeshes.count())
                  .arg(geoloc().Latitude)
                  .arg(geoloc().Longitude)
                  );

        foreach (CBoundedMeshInstances* pBounded, m_vBoundedMeshes)
        {
            delete pBounded;
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::setTerrain(QSP<CTerrain> value, bool bGenerateNow)
{
    m_pTerrain = value;

    if (m_pTerrain && m_pTerrain->level() < 2)
    {
        if (bGenerateNow)
        {
            work();
        }
        else
        {
            // Add this to workers for detail generation
            CWorkerManager::getInstance()->addWorker(this);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::setWater(QSP<CTerrain> value)
{
    m_pWater = value;
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::setGeoloc(CGeoloc value)
{
    CComponent::setGeoloc(value);

    computeWorldTransform();

    m_gOriginalGeoloc = value;
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::setSize(CGeoloc value)
{
    m_gSize = value;
    m_gOriginalSize = value;
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::build()
{
    CGeoloc gStart(
                geoloc().Latitude - m_gSize.Latitude * 0.5,
                geoloc().Longitude - m_gSize.Longitude * 0.5,
                0.0
                );

    CGeoloc gEnd(
                geoloc().Latitude + m_gSize.Latitude * 0.5,
                geoloc().Longitude + m_gSize.Longitude * 0.5,
                0.0
                );

    CMaterial* pMaterial = m_pAutoTerrain->material();

    if (pMaterial != nullptr)
    {
        gStart = pMaterial->transformGeoloc(gStart);
        gEnd = pMaterial->transformGeoloc(gEnd);
    }

    m_gSize = CGeoloc(gEnd.Latitude - gStart.Latitude, gEnd.Longitude - gStart.Longitude, 0.0);

    CComponent::setGeoloc(CGeoloc(gStart.Latitude + m_gSize.Latitude * 0.5, gStart.Longitude + m_gSize.Longitude * 0.5, 0.0));

    computeWorldTransform();

    m_bWorldBounds = createBoundsForTerrain(geoloc(), m_gSize);
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CWorldChunk::bounds()
{
    if (m_pTerrain && m_pTerrain->isOK())
    {
        return m_pTerrain->bounds();
    }

    return m_bWorldBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CWorldChunk::worldBounds()
{
    return m_bWorldBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CWorldChunk::buildWorldBounds()
{
    return m_bWorldBounds;
}


//-------------------------------------------------------------------------------------------------

void CWorldChunk::clearLinks(C3DScene* pScene)
{
    CComponent::clearLinks(pScene);

    m_pTerrain.reset();
    m_pWater.reset();

    foreach (QString sName, m_vBushMeshes.keys())
    {
        m_vBushMeshes[sName].reset();
    }

    m_vBushMeshes.clear();
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::update(double dDeltaTime)
{
    if (m_pTerrain && m_pTerrain->isOK())
    {
        m_pTerrain->update(dDeltaTime);
    }

    if (m_pWater && m_pWater->isOK())
    {
        m_pWater->update(dDeltaTime);
    }
}

//-------------------------------------------------------------------------------------------------

double CWorldChunk::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (m_pTerrain && m_pTerrain->isOK())
    {
        return m_pTerrain->getHeightAt(gPosition, pRigidness);
    }

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::flatten(const CGeoloc& gPosition, double dRadius)
{
    if (m_pTerrain && m_pTerrain->isOK())
    {
        return m_pTerrain->flatten(gPosition, dRadius);
    }
}

//-------------------------------------------------------------------------------------------------

bool CWorldChunk::operator < (const CWorldChunk& other) const
{
    return m_dDistance < other.m_dDistance;
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::paint(CRenderContext* pContext)
{
    // CVector3 vPosition = pContext->internalCameraMatrix() * worldBounds().center();
    // double dRadius = worldBounds().radius();

    setUsedNow();

    if (pContext->scene()->boundsOnly())
    {
        worldBounds().addSegments(pContext->scene());
    }
    else
    {
        // Paint chunk if containing sphere is within the viewing frustum
        // if (pContext->scene()->getFrustumCheck() == false || pContext->camera()->contains(vPosition, dRadius))
        {
            // pContext->tStatistics.m_iNumFrustumTests++;

            if (m_pTerrain && m_pTerrain->isOK())
            {
                CMaterial* pMaterial = m_pAutoTerrain->material();

                CTiledMaterial* pTiled = dynamic_cast<CTiledMaterial*>(pMaterial);

                if (pTiled != nullptr)
                {
                    pTiled->setCurrentPositionAndLevel(m_gOriginalGeoloc, m_pTerrain->level());
                }

                m_pTerrain->paint(pContext);
            }

            if (m_pWater && m_pWater->isOK())
            {
                glDisable(GL_CULL_FACE);

                m_pWater->paint(pContext);

                glEnable(GL_CULL_FACE);
            }

            if (m_bOK)
            {
                foreach (CBoundedMeshInstances* pBoundedMeshInstance, m_vBoundedMeshes)
                {
                    pBoundedMeshInstance->paint(pContext);
                }

                foreach (QString sBushName, m_vBushMeshes.keys())
                {
                    m_vBushMeshes[sBushName]->paint(pContext, this);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

bool CWorldChunk::drawable()
{
    bool bSelfDrawable = m_pTerrain && m_pTerrain->isOK();
    bool bChildrenDrawable = true;

    if (m_vChildren.count() == 0)
    {
        bChildrenDrawable = false;
    }
    else
    {
        foreach (QSP<CComponent> pChildComponent, m_vChildren)
        {
            QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

            if (pChild && pChild->drawable() == false)
            {
                bChildrenDrawable = false;
                break;
            }
        }
    }

    return bSelfDrawable || bChildrenDrawable;
}

//-------------------------------------------------------------------------------------------------

bool CWorldChunk::isEmpty()
{
    bool bWaitingForBuild = CWorkerManager::getInstance()->containsWorker(this);
    bool bSelfEmpty = (bWaitingForBuild == false && m_pTerrain == nullptr);
    bool bChildrenEmpty = true;

    foreach (QSP<CComponent> pChildComponent, m_vChildren)
    {
        QSP<CWorldChunk> pChild = QSP_CAST(CWorldChunk, pChildComponent);

        if (pChild && pChild->isEmpty() == false)
        {
            bChildrenEmpty = false;
            break;
        }
    }

    return bSelfEmpty && bChildrenEmpty;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CWorldChunk::createBoundsForTerrain(CGeoloc gPosition, CGeoloc gSize, double dAltitudeFactor)
{
    // Compute minimum and maximum altitude
    double dMinAltitude = gSize.Latitude * -10000.0 * dAltitudeFactor;
    double dMaxAltitude = gSize.Latitude * 100000.0 * dAltitudeFactor;

    // Set initial bound vectors
    CVector3 vMinimum( 999999999.0,  999999999.0,  999999999.0);
    CVector3 vMaximum(-999999999.0, -999999999.0, -999999999.0);

    // Create temporary vertices covering the patch
    QVector<CGeoloc> vGeos;

    for (double dLat = -0.5; dLat <= 0.5; dLat += 0.1)
    {
        for (double dLon = -0.5; dLon <= 0.5; dLon += 0.1)
        {
            vGeos.append(CGeoloc(gPosition.Latitude + gSize.Latitude * dLat, gPosition.Longitude + gSize.Longitude * dLon, dMinAltitude));
            vGeos.append(CGeoloc(gPosition.Latitude + gSize.Latitude * dLat, gPosition.Longitude + gSize.Longitude * dLon, dMaxAltitude));
        }
    }

    // Build bounds using temporary vertices
    foreach (CGeoloc vGeo, vGeos)
    {
        CVector3 vSummit = vGeo.toVector3();

        if (vSummit.X < vMinimum.X) vMinimum.X = vSummit.X;
        if (vSummit.Y < vMinimum.Y) vMinimum.Y = vSummit.Y;
        if (vSummit.Z < vMinimum.Z) vMinimum.Z = vSummit.Z;
        if (vSummit.X > vMaximum.X) vMaximum.X = vSummit.X;
        if (vSummit.Y > vMaximum.Y) vMaximum.Y = vSummit.Y;
        if (vSummit.Z > vMaximum.Z) vMaximum.Z = vSummit.Z;
    }

    return CBoundingBox(vMinimum, vMaximum);
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::clearTerrain()
{
    CGeoloc gChunkPosition = geoloc();

    if (m_pTerrain != nullptr)
    {
        LOG_METHOD_DEBUG(QString("Deleting CWorldChunk::m_pTerrain (%1, %2, %3)")
                  .arg(gChunkPosition.Latitude)
                  .arg(gChunkPosition.Longitude)
                  .arg(m_pTerrain->name())
                  );

        m_pTerrain.reset();
    }

    if (m_pWater != nullptr)
    {
        LOG_METHOD_DEBUG(QString("Deleting CWorldChunk::m_pWater (%1, %2, %3)")
                  .arg(gChunkPosition.Latitude)
                  .arg(gChunkPosition.Longitude)
                  .arg(m_pWater->name())
                  );

        m_pWater.reset();
    }
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::work()
{
    while (true)
    {
        if (m_pTerrain == nullptr) return;
        if (m_bStopRequested) return;
        if (m_pTerrain->isOK() == true) break;
        msleep(50);
    }

    if (m_pTerrain != nullptr)
    {
        // Create bounded containers
        {
            CGeoloc gStart(geoloc().Latitude - m_gSize.Latitude * 0.5, geoloc().Longitude - m_gSize.Longitude * 0.5, 0.0);

            double dSpanZ = m_gSize.Latitude / (double) NUM_CONTAINERS;
            double dSpanX = m_gSize.Longitude / (double) NUM_CONTAINERS;

            CGeoloc gSize(dSpanZ, dSpanX, 0.0);

            for (int iZ = 0; iZ < NUM_CONTAINERS; iZ++)
            {
                for (int iX = 0; iX < NUM_CONTAINERS; iX++)
                {
                    CGeoloc gPosition(
                                gStart.Latitude + ((double) iZ * dSpanZ) + (dSpanZ * 0.5),
                                gStart.Longitude + ((double) iX * dSpanX) + (dSpanX * 0.5),
                                0.0
                                );

                    CBoundingBox bBox = createBoundsForTerrain(gPosition, gSize, 1000.0);

                    LOG_METHOD_DEBUG(QString("Creating CWorldChunk::CBoundedMeshInstances for tile at lat %1, lon %2")
                              .arg(geoloc().Latitude)
                              .arg(geoloc().Longitude)
                              );

                    CBoundedMeshInstances* pBounded = new CBoundedMeshInstances(m_pScene);
                    pBounded->setGeoloc(gPosition);
                    pBounded->setBounds(bBox);

                    m_vBoundedMeshes.append(pBounded);
                }
            }
        }

        if (m_pScene->shaderQuality() > 0.7)
        {
            foreach (QSP<CComponent> pComponent, m_pAutoTerrain->generators())
            {
                QSP<CGeometryGenerator> pGenerator = QSP_CAST(CGeometryGenerator, pComponent);

                if (pGenerator != nullptr)
                {
                    pGenerator->generate(QSP<CWorldChunk>(this));
                }

                if (m_bStopRequested) return;
            }
        }
    }

    m_bOK = true;
}

//-------------------------------------------------------------------------------------------------

bool CWorldChunk::checkPositionFree(CGeoloc gPosition, double dRadius)
{
    /*
    QMutexLocker locker(&m_mMutex);

    vPosition.Y = 0.0;

    foreach (CBoundedMeshInstances* pBoundedMeshInstances, m_vMeshes)
    {
        CBoundingBox bBounds = pBoundedMeshInstances->getWorldBounds();

        if (bBounds.contains(vPosition))
        {
            foreach (CMeshInstance* pMeshInstance, pBoundedMeshInstances->getMeshes())
            {
                Vector3 vOtherPosition = pMeshInstance->getOriginPosition();
                vOtherPosition.Y = 0.0;
                CBoundingBox bOtherBox = pMeshInstance->getBounds();
                double dOtherRadius = ((bOtherBox.maximum() - bOtherBox.minimum()).getMagnitude()) / 2.0;
                double dDistance = (vOtherPosition - vPosition).getMagnitude();
                if (dDistance < (dRadius + dOtherRadius)) return false;
            }
        }
    }
    */

    return true;
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CWorldChunk::intersect(Math::CRay3 ray)
{
    if (m_pTerrain && m_pTerrain->isOK())
    {
        RayTracingResult dResult = worldBounds().intersect(ray);

        if (dResult.m_dDistance < Q3D_INFINITY)
        {
            return m_pTerrain->intersect(ray);
        }
    }

    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

bool CWorldChunk::isReallyExpendable() const
{
    return m_pTerrain != nullptr && m_pTerrain->isOK();
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CWorldChunk]"));
    dumpIdent(stream, iIdent, QString("Original geoloc : %1").arg(m_gOriginalGeoloc.toString()));
    dumpIdent(stream, iIdent, QString("Original size : %1").arg(m_gOriginalSize.toString()));
    dumpIdent(stream, iIdent, QString("Size : %1").arg(m_gSize.toString()));
    dumpIdent(stream, iIdent, QString("Last used seconds : %1").arg(m_tLastUsed.secsTo(QDateTime::currentDateTime())));
    dumpIdent(stream, iIdent, QString("Distance : %1").arg(m_dDistance));
    dumpIdent(stream, iIdent, QString("Terrain :"));

    dumpOpenBlock(stream, iIdent); iIdent++;
    if (m_pTerrain != nullptr) m_pTerrain->dump(stream, iIdent);
    iIdent--; dumpCloseBlock(stream, iIdent);

    dumpIdent(stream, iIdent, QString("Bounded meshes :"));

    dumpOpenBlock(stream, iIdent); iIdent++;
    foreach (CBoundedMeshInstances* pBounded, m_vBoundedMeshes)
    {
        pBounded->dump(stream, iIdent);
    }
    iIdent--; dumpCloseBlock(stream, iIdent);

    CComponent::dump(stream, iIdent);
}
