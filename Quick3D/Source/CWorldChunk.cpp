
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

//-------------------------------------------------------------------------------------------------

#define NUM_CONTAINERS	8

//-------------------------------------------------------------------------------------------------

int CWorldChunk::m_iNumWorldChunks = 0;

//-------------------------------------------------------------------------------------------------

CWorldChunk::CWorldChunk(C3DScene* pScene, CAutoTerrain* pAutoTerrain, CHeightField* pContainer)
    : CComponent(pScene)
    , m_pAutoTerrain(pAutoTerrain)
    , m_pTerrain(NULL)
    , m_pWater(NULL)
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

    {
        QMutexLocker locker(&m_mMutex);

        foreach (CBoundedMeshInstances* pBounded, m_vMeshes)
        {
            delete pBounded;
        }

        foreach (QString sKey, m_vBushMeshes.keys())
        {
            delete m_vBushMeshes[sKey];
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::setTerrain(QSP<CTerrain> value, bool bGenerateNow)
{
    m_pTerrain = value;

    if (m_pTerrain && m_pTerrain->getLevel() < 2)
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

CBoundingBox CWorldChunk::getBounds()
{
    if (m_pTerrain && m_pTerrain->isOK())
    {
        return m_pTerrain->getBounds();
    }

    return m_bWorldBounds;
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CWorldChunk::getWorldBounds()
{
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
    CVector3 vPosition = pContext->internalCameraMatrix() * getWorldBounds().center();
    double dRadius = getWorldBounds().radius();

    m_tLastUsed = QDateTime::currentDateTime();

    /*
    if (vPosition.getMagnitude() < 10000.0)
    {
        getWorldBounds().addSegments(pContext->scene());
    }
    */

    // Paint chunk if containing sphere is within the viewing frustum
    if (pContext->scene()->getFrustumCheck() == false || pContext->camera()->contains(vPosition, dRadius))
    {
        if (m_pTerrain && m_pTerrain->isOK())
        {
            CMaterial* pMaterial = m_pAutoTerrain->getMaterial();

            CTiledMaterial* pTiled = dynamic_cast<CTiledMaterial*>(pMaterial);

            if (pTiled != NULL)
            {
                pTiled->setCurrentPositionAndLevel(m_gOriginalGeoloc, m_pTerrain->getLevel());
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
            foreach (CBoundedMeshInstances* pBoundedMeshInstance, m_vMeshes)
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
    bool bSelfEmpty = (bWaitingForBuild == false && (!m_pTerrain));
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
    CGeoloc gChunkPosition = getGeoloc();

    if (m_pTerrain)
    {
        LOG_DEBUG(QString("Deleting terrain for chunk at %1, %2 (%3)")
                  .arg(gChunkPosition.Latitude)
                  .arg(gChunkPosition.Longitude)
                  .arg(m_pTerrain->getName())
                  );

        m_pTerrain.reset();
    }

    if (m_pWater)
    {
        m_pWater.reset();
    }
}

//-------------------------------------------------------------------------------------------------

void CWorldChunk::work()
{
    while (true)
    {
        if (!m_pTerrain) return;
        if (m_bStopRequested) return;
        if (m_pTerrain->isOK() == true) break;
        msleep(50);
    }

    if (m_pTerrain)
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
        for (int iVegetIndex = 0; iVegetIndex < m_pAutoTerrain->getVegetation().count(); iVegetIndex++)
        {
            CVegetation* pVegetation = m_pAutoTerrain->getVegetation()[iVegetIndex];

            double dSpread = pVegetation->m_dSpread * ((double) m_pTerrain->getLevel() + 1.0);
            double dAltitude_Trees = 10.0;

            CGeoloc gStart(getGeoloc().Latitude - m_gSize.Latitude * 0.5, getGeoloc().Longitude - m_gSize.Longitude * 0.5, 0.0);

            double dLatStart = fmod(gStart.Latitude, dSpread);
            double dLonStart = fmod(gStart.Longitude, dSpread);

            if (dLatStart < 0.0) dLatStart -= dSpread * 0.5;
            if (dLonStart < 0.0) dLonStart -= dSpread * 0.5;

            CGeoloc gStartOffset = CGeoloc(gStart.Latitude - dLatStart, gStart.Longitude - dLonStart, 0.0);

            for (double dLat = gStartOffset.Latitude; dLat < gStartOffset.Latitude + m_gSize.Latitude; dLat += dSpread)
            {
                for (double dLon = gStartOffset.Longitude; dLon < gStartOffset.Longitude + m_gSize.Longitude; dLon += dSpread)
                {
                    if (
                            dLat > gStart.Latitude && dLat < gStart.Latitude + m_gSize.Latitude &&
                            dLon > gStart.Longitude && dLon < gStart.Longitude + m_gSize.Longitude
                            )
                    {
                        CGeoloc gPosition(dLat, dLon, 0.0);
                        // CVector3 vPosition = gPosition.toVector3();

                        /*
                        CVector3 vLatDisplace = (vPosition + CVector3(iVegetIndex, iVegetIndex, iVegetIndex)) * 0.0001;
                        CVector3 vLonDisplace = (vPosition - CVector3(iVegetIndex, iVegetIndex, iVegetIndex)) * 0.0002;

                        double dLatNoise = perlin->getNoise(vLatDisplace);
                        double dLonNoise = perlin->getNoise(vLonDisplace);

                        gPosition.Latitude += dLatNoise * (dSpread * 0.25);
                        gPosition.Longitude += dLonNoise * (dSpread * 0.25);
                        */

                        // gPosition.Latitude += (((double) qrand() / (double) RAND_MAX) - 0.5) * dSpread;
                        // gPosition.Longitude += (((double) qrand() / (double) RAND_MAX) - 0.5) * dSpread;

                        double dLandscapeValue = pVegetation->m_pFunction->process(perlin, gPosition.toVector3(), CAxis());

                        if (dLandscapeValue > 0.0)
                        {
                            double dRigidness = 0.0;
                            gPosition.Altitude = m_pTerrain->getHeightAt(gPosition, &dRigidness);

                            if (gPosition.Altitude >= dAltitude_Trees)
                            {
                                switch (pVegetation->m_eType)
                                {
                                    case CVegetation::evtTree:
                                        placeTree(gPosition, 5.0, iVegetIndex);
                                        break;

                                    case CVegetation::evtBush:
                                        placeBush(gPosition, 5.0, iVegetIndex);
                                        break;

                                    default:
                                        break;
                                }
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

void CWorldChunk::placeTree(CGeoloc gPosition, double dRadius, int iVegetIndex)
{
    if (checkPositionFree(gPosition, dRadius))
    {
        CMeshInstance* pMeshInstance = m_pAutoTerrain->getVegetation()[iVegetIndex]->m_pMesh->clone();

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

void CWorldChunk::placeBush(CGeoloc gPosition, double dRadius, int iVegetIndex)
{
    if (m_pAutoTerrain->getVegetation()[iVegetIndex]->m_pMaterial)
    {
        QString sMaterialName = m_pAutoTerrain->getVegetation()[iVegetIndex]->m_pMaterial->getName();

        if (sMaterialName.isEmpty() == false)
        {
            if (m_vBushMeshes.contains(sMaterialName) == false)
            {
                CMeshGeometry* pBushMesh = new CMeshGeometry(m_pScene, 100000.0);

                pBushMesh->setGLType(GL_POINTS);
                pBushMesh->setMaterial(m_pAutoTerrain->getVegetation()[iVegetIndex]->m_pMaterial);

                m_vBushMeshes[sMaterialName] = pBushMesh;
            }

            CVector3 vGeocentricPosition = gPosition.toVector3();
            CVector3 vPosition = vGeocentricPosition - getWorldPosition();

            CVertex newVertex(vPosition);
            newVertex.setNormal(vGeocentricPosition.Normalize());
            m_vBushMeshes[sMaterialName]->getVertices().append(newVertex);
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

RayTracingResult CWorldChunk::intersect(Math::CRay3 ray)
{
    if (m_pTerrain && m_pTerrain->isOK())
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

bool CWorldChunk::isReallyExpendable() const
{
    return m_pTerrain && m_pTerrain->isOK();
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
