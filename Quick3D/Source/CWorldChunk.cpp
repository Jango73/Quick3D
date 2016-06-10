
// Fondations
#include "CLogManager.h"

// Application
#include "CWorkerManager.h"
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CAutoTerrain.h"
#include "CWorldChunk.h"
#include "CTiledMaterial.h"
#include "CPerlin.h"

using namespace Math;

#define NUM_CONTAINERS	8

//-------------------------------------------------------------------------------------------------

int CWorldChunk::m_iNumWorldChunks = 0;

//-------------------------------------------------------------------------------------------------

CWorldChunk::CWorldChunk(C3DScene* pScene, CAutoTerrain* pAutoTerrain, CHeightField* pContainer)
    : CComponent(pScene)
    , m_pAutoTerrain(pAutoTerrain)
    , m_pTerrain(NULL)
    , m_pWater(NULL)
    , m_mBoundingBoxVisual(NULL)
    , m_pContainer(pContainer)
    , m_mMutex(QMutex::Recursive)
    , m_dDistance(0.0)
    , m_bOK(false)
{
    m_iNumWorldChunks++;

    m_tLastUsed = QDateTime::currentDateTime();
}

//-------------------------------------------------------------------------------------------------

CWorldChunk::~CWorldChunk()
{
    m_iNumWorldChunks--;

    // Remove this from workers
    CWorkerManager::getInstance()->removeWorker(this);

    if (m_pTerrain != NULL)
    {
        delete m_pTerrain;
    }

    if (m_pWater != NULL)
    {
        delete m_pWater;
    }

    if (m_mBoundingBoxVisual != NULL)
    {
        delete m_mBoundingBoxVisual;
    }

    {
        QMutexLocker locker(&m_mMutex);

        foreach (CBoundedMeshInstances* pBounded, m_vMeshes)
        {
            delete pBounded;
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::setTerrain(CTerrain* value, bool bGenerateNow)
{
    m_pTerrain = value;

    if (m_pTerrain != NULL && m_pTerrain->getLevel() == 0)
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

void CWorldChunk::setWater(CTerrain* value)
{
    m_pWater = value;
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::setGeoloc(CGeoloc gGeoloc)
{
    CComponent::setGeoloc(gGeoloc);

    computeWorldTransform();

    m_gOriginalGeoloc = gGeoloc;
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
                getGeoloc().Latitude - m_gSize.Latitude * 0.5,
                getGeoloc().Longitude - m_gSize.Longitude * 0.5,
                0.0
                );

    CGeoloc gEnd(
                getGeoloc().Latitude + m_gSize.Latitude * 0.5,
                getGeoloc().Longitude + m_gSize.Longitude * 0.5,
                0.0
                );

    CMaterial* pMaterial = m_pAutoTerrain->getMaterial();

    if (pMaterial)
    {
        gStart = pMaterial->transformGeoloc(gStart);
        gEnd = pMaterial->transformGeoloc(gEnd);
    }

    m_gSize = CGeoloc(gEnd.Latitude - gStart.Latitude, gEnd.Longitude - gStart.Longitude, 0.0);

    CComponent::setGeoloc(CGeoloc(gStart.Latitude + m_gSize.Latitude * 0.5, gStart.Longitude + m_gSize.Longitude * 0.5, 0.0));

    computeWorldTransform();

    m_bWorldBounds = createBoundsForTerrain(getGeoloc(), m_gSize);
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CWorldChunk::getBounds() const
{
    if (m_pTerrain && m_pTerrain->isOK())
    {
        return m_pTerrain->getBounds();
    }

    return m_bWorldBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CWorldChunk::getWorldBounds() const
{
    if (m_pTerrain && m_pTerrain->isOK())
    {
        return m_pTerrain->getWorldBounds();
    }

    return m_bWorldBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CWorldChunk::getBuildWorldBounds()
{
    return m_bWorldBounds;
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
    if (m_pTerrain && m_pTerrain->isOK()) return m_pTerrain->getHeightAt(gPosition, pRigidness);

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::flatten(const CGeoloc& gPosition, double dRadius)
{
    if (m_pTerrain && m_pTerrain->isOK()) return m_pTerrain->flatten(gPosition, dRadius);
}

//-------------------------------------------------------------------------------------------------

bool CWorldChunk::operator < (const CWorldChunk& other)
{
    return m_dDistance < other.m_dDistance;
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::paint(CRenderContext* pContext, ETerrainType eType)
{
    CVector3 vPosition = pContext->internalCameraMatrix() * getWorldBounds().center();
    double dRadius = getWorldBounds().radius();

    m_tLastUsed = QDateTime::currentDateTime();

    // En cas de mode debug, on créé le mesh de la boite englobante
    if (pContext->scene()->getDebugMode())
    {
        if (pContext->scene()->getBoundsOnly())
        {
            CBoundingBox bounds = getBounds();

            if (m_mBoundingBoxVisual == NULL)
            {
                m_mBoundingBoxVisual = new CBox(pContext->scene(), 100000000.0);
                m_mBoundingBoxVisual->setBounds(bounds.minimum(), bounds.maximum());
                m_mBoundingBoxVisual->setGeoloc(getGeoloc());
            }
        }
    }

    // Paint chunk if containing sphere is within the viewing frustum
    if (pContext->scene()->getFrustumCheck() == false || pContext->camera()->contains(vPosition, dRadius))
    {
        switch (eType)
        {
            case ttGround:
            {
                if (pContext->scene()->getDebugMode() == false || pContext->scene()->getBoundsOnly() == false)
                {
                    if (m_pTerrain != NULL && m_pTerrain->isOK())
                    {
                        CMaterial* pMaterial = m_pAutoTerrain->getMaterial();

                        CTiledMaterial* pTiled = dynamic_cast<CTiledMaterial*>(pMaterial);

                        if (pTiled != NULL)
                        {
                            pTiled->setCurrentPositionAndLevel(m_gOriginalGeoloc, m_pTerrain->getLevel());
                        }

                        m_pTerrain->paint(pContext);
                    }
                }

                if (pContext->scene()->getDebugMode() && m_mBoundingBoxVisual != NULL)
                {
                    m_mBoundingBoxVisual->paint(pContext);
                }
            }
                break;

            case ttWater:
            {
                if (pContext->scene()->getDebugMode() == false || pContext->scene()->getBoundsOnly() == false)
                {
                    if (m_pWater != NULL && m_pWater->isOK())
                    {
                        glDisable(GL_CULL_FACE);

                        m_pWater->paint(pContext);

                        glEnable(GL_CULL_FACE);
                    }
                }
            }
                break;

            case ttVegetation:
            {
                if (pContext->scene()->getDebugMode() == false || pContext->scene()->getBoundsOnly() == false)
                {
                    if (m_bOK)
                    {
                        foreach (CBoundedMeshInstances* pBoundedMeshInstance, m_vMeshes)
                        {
                            pBoundedMeshInstance->paint(pContext);
                        }
                    }
                }
            }
                break;
        }
    }
}

//-------------------------------------------------------------------------------------------------

bool CWorldChunk::drawable()
{
    bool bSelfDrawable = m_pTerrain != NULL && m_pTerrain->isOK();
    bool bChildrenDrawable = true;

    if (m_vChildren.count() == 0)
    {
        bChildrenDrawable = false;
    }
    else
    {
        foreach (CComponent* pChildComponent, m_vChildren)
        {
            CWorldChunk* pChild = dynamic_cast<CWorldChunk*>(pChildComponent);

            if (pChild->drawable() == false)
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
    bool bSelfEmpty = (bWaitingForBuild == false && m_pTerrain == NULL);
    bool bChildrenEmpty = true;

    foreach (CComponent* pChildComponent, m_vChildren)
    {
        CWorldChunk* pChild = dynamic_cast<CWorldChunk*>(pChildComponent);

        if (pChild->isEmpty() == false)
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
    CGeoloc gChunkPosition = getGeoloc();

    if (m_pTerrain != NULL)
    {
        LOG_DEBUG(QString("Deleting terrain for chunk at %1, %2 (%3)")
                  .arg(gChunkPosition.Latitude)
                  .arg(gChunkPosition.Longitude)
                  .arg(m_pTerrain->getName())
                  );

        delete m_pTerrain;
        m_pTerrain = NULL;
    }

    if (m_pWater != NULL)
    {
        delete m_pWater;
        m_pWater = NULL;
    }
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::work()
{
    while (true)
    {
        if (m_pTerrain == NULL) return;
        if (m_bStopRequested) return;
        if (m_pTerrain->isOK() == true) break;
        msleep(50);
    }

    if (m_pTerrain != NULL)
    {
        CPerlin* perlin = CPerlin::getInstance();

        // Create bounded containers
        {
            CGeoloc gStart(getGeoloc().Latitude - m_gSize.Latitude * 0.5, getGeoloc().Longitude - m_gSize.Longitude * 0.5, 0.0);

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

                    CBoundedMeshInstances* pBounded = new CBoundedMeshInstances(m_pScene);
                    pBounded->setGeoloc(gPosition);
                    pBounded->setBounds(bBox);

                    m_vMeshes.append(pBounded);
                }
            }
        }

        // Generate vegetation
        for (int iTreeIndex = 0; iTreeIndex < m_pAutoTerrain->getVegetation().count(); iTreeIndex++)
        {
            CVegetation* pVegetation = m_pAutoTerrain->getVegetation()[iTreeIndex];

            double dAltitude_Trees = 10.0;

            CGeoloc gStart(getGeoloc().Latitude - m_gSize.Latitude * 0.5, getGeoloc().Longitude - m_gSize.Longitude * 0.5, 0.0);

            double dLatStart = fmod(gStart.Latitude, pVegetation->m_dSpread);
            double dLonStart = fmod(gStart.Longitude, pVegetation->m_dSpread);

            if (dLatStart < 0.0) dLatStart -= pVegetation->m_dSpread * 0.5;
            if (dLonStart < 0.0) dLonStart -= pVegetation->m_dSpread * 0.5;

            CGeoloc gStartOffset = CGeoloc(gStart.Latitude - dLatStart, gStart.Longitude - dLonStart, 0.0);

            for (double dLat = gStartOffset.Latitude; dLat < gStartOffset.Latitude + m_gSize.Latitude; dLat += pVegetation->m_dSpread)
            {
                for (double dLon = gStartOffset.Longitude; dLon < gStartOffset.Longitude + m_gSize.Longitude; dLon += pVegetation->m_dSpread)
                {
                    if (
                            dLat > gStart.Latitude && dLat < gStart.Latitude + m_gSize.Latitude &&
                            dLon > gStart.Longitude && dLon < gStart.Longitude + m_gSize.Longitude
                            )
                    {
                        CGeoloc gPosition(dLat, dLon, 0.0);
                        CVector3 vPosition = gPosition.toVector3();

                        gPosition.Latitude += perlin->getNoise((vPosition + CVector3(iTreeIndex, iTreeIndex, iTreeIndex)) * 0.5) * (pVegetation->m_dSpread * 0.5);
                        gPosition.Longitude += perlin->getNoise((vPosition + CVector3(iTreeIndex, iTreeIndex, iTreeIndex)) * 0.5) * (pVegetation->m_dSpread * 0.5);

                        vPosition = gPosition.toVector3();

                        double dLandscapeValue = pVegetation->m_pFunction->process(perlin, vPosition, CAxis());

                        if (dLandscapeValue > 0.0)
                        {
                            double dRigidness = 0.0;
                            double dAltitude = m_pTerrain->getHeightAt(vPosition, &dRigidness);

                            if (dAltitude >= dAltitude_Trees)
                            {
                                gPosition = CGeoloc(dLat, dLon, dAltitude);

                                /*
                                gPosition = CGeoloc(
                                    gPosition.Latitude,
                                    gPosition.Longitude,
                                    dAltitude
                                    );
                                    */

                                placeTree(gPosition, 5.0, iTreeIndex);
                            }
                        }
                    }
                }

                if (m_bStopRequested) return;
            }
        }
    }

    m_bOK = true;
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::placeTree(CGeoloc gPosition, double dRadius, int iTreeIndex)
{
    if (checkPositionFree(gPosition, dRadius))
    {
        CMeshInstance* pMeshInstance = m_pAutoTerrain->getVegetation()[iTreeIndex]->m_pMesh->clone();

        if (pMeshInstance != NULL)
        {
            pMeshInstance->setGeoloc(gPosition);
            pMeshInstance->setOriginRotation(CVector3(0.0, ((double) rand() / 32768.0) * Math::Pi * 2.0, 0.0));
            pMeshInstance->computeWorldTransform();

            foreach (CBoundedMeshInstances* pBounded, m_vMeshes)
            {
                if (pBounded->getWorldBounds().contains(gPosition, dRadius))
                {
                    pBounded->add(pMeshInstance);
                    return;
                }
            }

            delete pMeshInstance;
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::placeBuilding(CGeoloc gPosition, double dRadius, int iBuildingIndex)
{
    if (checkPositionFree(gPosition, dRadius))
    {
        if (m_pContainer) m_pContainer->flatten(gPosition, dRadius);

        /*
        CMeshInstance* pMeshInstance = CBuildingGenerator::getInstance()->getBuilding(eType, gPosition, dRadius, dSeed);

        if (pMeshInstance != NULL)
        {
            foreach (CBoundedMeshInstances* pBounded, m_vMeshes)
            {
                if (pBounded->getWorldBounds().contains(gPosition))
                {
                    pBounded->add(pMeshInstance);
                    return;
                }
            }

            delete pMeshInstance;
        }
        */
    }
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

RayTracingResult CWorldChunk::intersect(Math::CRay3 ray) const
{
    if (m_pTerrain != NULL && m_pTerrain->isOK())
    {
        RayTracingResult dResult = getWorldBounds().intersect(ray);

        if (dResult.m_dDistance < Q3D_INFINITY)
        {
            return m_pTerrain->intersect(ray);
        }
    }

    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CWorldChunk]"));
    dumpIdent(stream, iIdent, QString("Original geoloc : %1").arg(m_gOriginalGeoloc.toString()));
    dumpIdent(stream, iIdent, QString("Original size : %1").arg(m_gOriginalSize.toString()));
    dumpIdent(stream, iIdent, QString("Size : %1").arg(m_gSize.toString()));
    dumpIdent(stream, iIdent, QString("Last used : %1").arg(m_tLastUsed.toString()));
    dumpIdent(stream, iIdent, QString("Distance : %1").arg(m_dDistance));
    dumpIdent(stream, iIdent, QString("Terrain :"));

    dumpOpenBlock(stream, iIdent); iIdent++;
    if (m_pTerrain) m_pTerrain->dump(stream, iIdent);
    iIdent--; dumpCloseBlock(stream, iIdent);

    CComponent::dump(stream, iIdent);
}
