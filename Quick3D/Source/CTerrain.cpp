
// qt-plus
#include "CLogger.h"

// Application
#include "CWorkerManager.h"
#include "CRessourcesManager.h"
#include "CTerrain.h"
#include "CWaterMaterial.h"
#include "CTiledMaterial.h"
#include "C3DScene.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CInterpolator<double> CTerrain::m_iAltitudes_Sand;
CInterpolator<double> CTerrain::m_iAltitudes_Dirt;
CInterpolator<double> CTerrain::m_iAltitudes_Grass;
CInterpolator<double> CTerrain::m_iAltitudes_Snow;

//-------------------------------------------------------------------------------------------------

CTerrain::CTerrain(
        C3DScene* pScene,
        CHeightField* pHeights,
        CGeoloc gOriginalGeoloc,
        CGeoloc gOriginalSize,
        CGeoloc gGeoloc,
        CGeoloc gSize,
        int iPoints,
        int iLevel,
        int iMaxLevel,
        bool bIsWater,
        bool bGenerateNow
        )
    : CComponent(pScene)
    , CHeightField(1.0)
    , m_pHeights(pHeights)
    , m_gOriginalGeoloc(gOriginalGeoloc)
    , m_gOriginalSize(gOriginalSize)
    , m_gSize(gSize)
    , m_pMesh(nullptr)
    , m_iNumPoints(iPoints)
    , m_iLevel(iLevel)
    , m_iMaxLevel(iMaxLevel)
    , m_bAllHeightsOverSea(false)
    , m_bIsWater(bIsWater)
    , m_bOK(false)
{
    CComponent::incComponentCounter(ClassName_CTerrain);

    QString sGeolocName = QString("_%1_%2").arg(m_gOriginalGeoloc.Latitude).arg(m_gOriginalGeoloc.Longitude);

    setName(QString("Terrain%1").arg(sGeolocName));

    if (m_iNumPoints % 2 == 0) m_iNumPoints++;

    if (m_iAltitudes_Sand.count() == 0)
    {
        m_iAltitudes_Sand.addValue( 0.0, 1.0);
        m_iAltitudes_Sand.addValue(10.0, 0.0);

        m_iAltitudes_Dirt.addValue(10.0, 0.0);
        m_iAltitudes_Dirt.addValue(50.0, 1.0);

        m_iAltitudes_Grass.addValue(50.0, 0.0);
        m_iAltitudes_Grass.addValue(100.0, 1.0);
        m_iAltitudes_Grass.addValue(500.0, 1.0);
        m_iAltitudes_Grass.addValue(600.0, 0.0);

        m_iAltitudes_Snow.addValue(1000.0, 0.0);
        m_iAltitudes_Snow.addValue(1200.0, 1.0);
    }

    setGeoloc(gGeoloc);
    computeWorldTransform();

    // Create terrain mesh
    LOG_METHOD_DEBUG(QString("Creating CTerrain::m_pMesh (%1, %2, %3)")
              .arg(gOriginalGeoloc.Latitude)
              .arg(gOriginalGeoloc.Longitude)
              .arg(m_sName)
              );

    m_pMesh = new CMeshGeometry(pScene, 100000000.0, true);
    // m_pMesh->setName(QString("TerrainMesh%1").arg(sGeolocName));

    if (bIsWater == false)
    {
    }
    else
    {
        m_pMesh->setMaterial(pScene->ressourcesManager()->getWaterMaterial());
    }

    if (bGenerateNow)
    {
        work();
    }
    else
    {
        // Add this to workers for mesh generation
        CWorkerManager::getInstance()->addWorker(this);
    }
}

//-------------------------------------------------------------------------------------------------

CTerrain::~CTerrain()
{
    CComponent::decComponentCounter(ClassName_CTerrain);

    // Remove this from workers
    CWorkerManager::getInstance()->removeWorker(this);

    if (m_pMesh != nullptr)
    {
        delete m_pMesh;
    }

    foreach (CMeshGeometry* pSeam, m_vSeams)
    {
        delete pSeam;
    }
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CTerrain::bounds()
{
    if (m_pMesh != nullptr)
    {
        return m_pMesh->bounds();
    }

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CTerrain::worldBounds()
{
    if (m_pMesh != nullptr)
    {
        return m_pMesh->worldBounds(this);
    }

    return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

void CTerrain::setMaterial(QSP<CMaterial> pMaterial)
{
    m_pMesh->setMaterial(pMaterial);
}

//-------------------------------------------------------------------------------------------------

int CTerrain::getPointIndexForXZ (int iX, int iZ) const
{
    return (iZ * m_iNumPoints) + iX;
}

//-------------------------------------------------------------------------------------------------

int CTerrain::getFaceIndexForVertices(int v1, int v2, int v3, int v4) const
{
    if (m_pMesh != nullptr)
    {
        QString sKey = QString("%1%2%3%4")
                .arg(v1)
                .arg(v2)
                .arg(v3)
                .arg(v4)
                ;

        if (m_mVerticesToFace.contains(sKey))
        {
            return m_mVerticesToFace[sKey];
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

void CTerrain::paint(CRenderContext* pContext)
{
    m_pMesh->paint(pContext, this);
}

//-------------------------------------------------------------------------------------------------

double CTerrain::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    // Using terrain parameters
    // CPerlin* pPerlin = CPerlin::getInstance();
    // return m_pFunction->process(pPerlin, vPosition);

    if (pRigidness != nullptr)
    {
        *pRigidness = 1.0;
    }

    if (m_bOK == false)
    {
        return Q3D_INFINITY;
    }

    if (m_bIsWater)
    {
        if (
                m_pMesh != nullptr && m_pMesh->materials().count() > 0 &&
                dynamic_cast<CHeightField*>(m_pMesh->materials()[0].data()) != nullptr
                )
        {
            CHeightField* pField = dynamic_cast<CHeightField*>(m_pMesh->materials()[0].data());
            return pField->getHeightAt(gPosition, pRigidness);
        }

        return 0.0;
    }

    double dLatitudeDiff = Math::Angles::angleDifferenceDegree(gPosition.Latitude, geoloc().Latitude);
    double dLongitudeDiff = Math::Angles::angleDifferenceDegree(gPosition.Longitude, geoloc().Longitude);

    CGeoloc gLocalGeoloc(dLatitudeDiff, dLongitudeDiff, 0.0);
    CVector3 vCenter = geoloc().toVector3();
    CVector3 vLocal = gPosition.toVector3() - vCenter;
    CVector3 vUp = vCenter.normalized();

    int iX = (int) (((gLocalGeoloc.Longitude / (m_gSize.Longitude)) + 0.5) * (m_iNumPoints - 1));
    int iZ = (int) (((gLocalGeoloc.Latitude / (m_gSize.Latitude)) + 0.5) * (m_iNumPoints - 1));

    for (int iIterZ = iZ - 2; iIterZ <= iZ + 2; iIterZ++)
    {
        for (int iIterX = iX - 2; iIterX <= iX + 2; iIterX++)
        {
            int iIndex1 = getPointIndexForXZ(iIterX + 0, iIterZ + 0);
            int iIndex2 = getPointIndexForXZ(iIterX + 1, iIterZ + 0);
            int iIndex3 = getPointIndexForXZ(iIterX + 1, iIterZ + 1);
            int iIndex4 = getPointIndexForXZ(iIterX + 0, iIterZ + 1);

            if (
                    /*
                        // Skirt
                        iIndex1 >= 1 && iIndex2 >= 1 && iIndex3 >= 1 && iIndex4 >= 1 &&
                        iIndex1 < m_pMesh->getVertices().count() - 1 &&
                        iIndex2 < m_pMesh->getVertices().count() - 1 &&
                        iIndex3 < m_pMesh->getVertices().count() - 1 &&
                        iIndex4 < m_pMesh->getVertices().count() - 1
                        */
                    // No skirt
                    iIndex1 >= 0 && iIndex2 >= 0 && iIndex3 >= 0 && iIndex4 >= 0 &&
                    iIndex1 < m_pMesh->vertices().count() &&
                    iIndex2 < m_pMesh->vertices().count() &&
                    iIndex3 < m_pMesh->vertices().count() &&
                    iIndex4 < m_pMesh->vertices().count()
                    )
            {
                CVector3 v1(m_pMesh->vertices()[iIndex1].position());
                CVector3 v2(m_pMesh->vertices()[iIndex2].position());
                CVector3 v3(m_pMesh->vertices()[iIndex3].position());
                CVector3 v4(m_pMesh->vertices()[iIndex4].position());

                CRay3 ray(vLocal + vUp * 50000.0, vUp * -1.0);

                RayTracingResult dResult(0.0);

                int iFaceIndex = getFaceIndexForVertices(iIndex1, iIndex2, iIndex3, iIndex4);

                if (iFaceIndex != -1)
                {
                    dResult = CFace::intersectTriangle(
                                ray,
                                v1, v2, v3
                                );

                    if (dResult.m_dDistance < Q3D_INFINITY)
                    {
                        return CGeoloc(vCenter + ray.vOrigin + ray.vNormal * dResult.m_dDistance).Altitude;
                    }

                    dResult = CFace::intersectTriangle(
                                ray,
                                v1, v3, v4
                                );

                    if (dResult.m_dDistance < Q3D_INFINITY)
                    {
                        return CGeoloc(vCenter + ray.vOrigin + ray.vNormal * dResult.m_dDistance).Altitude;
                    }
                }
            }
        }
    }

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

void CTerrain::work()
{
    LOG_METHOD_DEBUG(QString("START : %1").arg(m_sName));

    m_bAllHeightsOverSea = true;
    int vVertexCount = 0;

    CPerlin* pPerlin = CPerlin::getInstance();

    // Initial patch
    if (m_bIsWater == false && m_iLevel < m_iMaxLevel / 2)
    {
        // Skirt
        // m_pMesh->createQuadPatch(m_iNumPoints, 1);
        // m_iNumPoints += 2;
        m_pMesh->createQuadPatch(m_iNumPoints, 0);
    }
    else
    {
        m_pMesh->createQuadPatch(m_iNumPoints, 0);
    }

    buildVerticesToFaceMap();

    CVector3 vFinalCenter = geoloc().toVector3();

    // Get the mesh's material
    CMaterial* pMaterial = nullptr;

    if (m_pMesh->materials().count() > 0)
    {
        pMaterial = m_pMesh->materials()[0].data();
    }

    CTiledMaterial* pTiledMaterial = dynamic_cast<CTiledMaterial*>(pMaterial);

    // Move vertices
    for (int iIndex = 0; iIndex < m_pMesh->vertices().count(); iIndex++)
    {
        CGeoloc gPosition;
        CVector3 vPosition = m_pMesh->vertices()[iIndex].position();

        gPosition.Latitude = m_gOriginalGeoloc.Latitude + vPosition.Z * m_gOriginalSize.Latitude;
        gPosition.Longitude = m_gOriginalGeoloc.Longitude + vPosition.X * m_gOriginalSize.Longitude;

        if (pMaterial != nullptr)
        {
            gPosition = pMaterial->transformGeoloc(gPosition);
        }

        /*
        // Skirt
        if (m_bIsWater == false && m_iLevel < m_iMaxLevel / 2)
        {
            if (
                gPosition.Latitude < geoloc().Latitude - m_gSize.Latitude * 0.5 ||
                gPosition.Latitude > geoloc().Latitude + m_gSize.Latitude * 0.5 ||
                gPosition.Longitude < geoloc().Longitude - m_gSize.Longitude * 0.5 ||
                gPosition.Longitude > geoloc().Longitude + m_gSize.Longitude * 0.5
                )
            {
                gPosition.Altitude = -m_gSize.Longitude * 100.0;
            }
        }
        */

        CAxis aNOLLAxis = gPosition.getNOLLAxis();

        m_pMesh->vertices()[iIndex].position() = gPosition.toVector3();
        m_pMesh->vertices()[iIndex].normal() = m_pMesh->vertices()[iIndex].position().normalized();
        m_pMesh->vertices()[iIndex].tangent() = aNOLLAxis.Front;
        m_pMesh->vertices()[iIndex].gravity() = m_pMesh->vertices()[iIndex].normal() * -1.0;

        if (pMaterial != nullptr && pTiledMaterial == nullptr)
        {
            m_pMesh->vertices()[iIndex].texCoord() = pMaterial->texCoords(gPosition, m_iLevel);
        }
        else if (m_bIsWater)
        {
            m_pMesh->vertices()[iIndex].texCoord().X = gPosition.Longitude;
            m_pMesh->vertices()[iIndex].texCoord().Y = gPosition.Altitude;
            m_pMesh->vertices()[iIndex].texCoord().Z = gPosition.Latitude;
        }

        if (m_bStopRequested)
        {
            return;
        }
    }

    // Loop over vertices
    for (int iIndex = 0; iIndex < m_pMesh->vertices().count(); iIndex++)
    {
        double dTerrainAltitude = 0.0;

        if (m_pHeights != nullptr)
        {
            // This test is important: with non-generated terrain, we don't want to load too much data in RAM
            // We therefore get an altitude only for levels that are close to sea (x < niveau max / 2)

            if (m_pHeights->isGenerated() || m_iLevel < m_iMaxLevel / 2)
            {
                dTerrainAltitude = m_pHeights->getHeightAt(
                            m_pMesh->vertices()[iIndex].position(),
                            CAxis(m_pMesh->vertices()[iIndex].tangent(), m_pMesh->vertices()[iIndex].normal()),
                            false
                            );
            }
            else
            {
                dTerrainAltitude = 0.0;
            }

            if (fabs(dTerrainAltitude - Q3D_INFINITY) < 0.01)
            {
                dTerrainAltitude = 0.0;
            }

            if (dTerrainAltitude < -20000.0)
            {
                dTerrainAltitude = -20000.0;
            }

            if (dTerrainAltitude < 0.01)
            {
                m_bAllHeightsOverSea = false;
            }

            m_pMesh->vertices()[iIndex].altitude() = dTerrainAltitude;
        }

        // If this is a water mesh, altitude is always 0
        if (m_bIsWater == true)
        {
            dTerrainAltitude = 0.0;
        }

        // Translate vertex to final position
        m_pMesh->vertices()[iIndex].position() =
                (m_pMesh->vertices()[iIndex].position() - vFinalCenter) +
                (m_pMesh->vertices()[iIndex].gravity() * -1.0) * dTerrainAltitude;

        vVertexCount++;

        if (m_bStopRequested)
        {
            return;
        }
    }

    m_pMesh->computeNormals();

    vVertexCount = 0;

    // Loop over vertices
    if (pMaterial != nullptr && pTiledMaterial == nullptr)
    {
        LOG_METHOD_DEBUG(QString("Setting up terrain textures for non-tiled material"));

        for (int iIndex = 0; iIndex < m_pMesh->vertices().count(); iIndex++)
        {
            if (m_bIsWater == false)
            {
                double dSlope = fabs(m_pMesh->vertices()[iIndex].normal().dot(
                                         m_pMesh->vertices()[iIndex].gravity() * -1.0
                                         ));

                dSlope = pow(dSlope, 4.0);

                double dStoneMix = pPerlin->noise_0_1((vFinalCenter + m_pMesh->vertices()[iIndex].position()) * 0.2);
                double dDirtMix = pPerlin->noise_0_1((vFinalCenter + m_pMesh->vertices()[iIndex].position()) * 0.3);
                double dGrassMix = pPerlin->noise_0_1((vFinalCenter + m_pMesh->vertices()[iIndex].position()) * 0.1);

                m_pMesh->vertices()[iIndex].setDiffuseTextureWeight(0, 1.0);

                m_pMesh->vertices()[iIndex].setDiffuseTextureWeight(0, dStoneMix);
                m_pMesh->vertices()[iIndex].setDiffuseTextureWeight(1, 1.0 - dStoneMix);

                m_pMesh->vertices()[iIndex].setDiffuseTextureWeight(2, m_iAltitudes_Dirt.getValue(m_pMesh->vertices()[iIndex].altitude()) * dSlope * dDirtMix);
                m_pMesh->vertices()[iIndex].setDiffuseTextureWeight(3, m_iAltitudes_Dirt.getValue(m_pMesh->vertices()[iIndex].altitude()) * dSlope * (1.0 - dDirtMix));

                m_pMesh->vertices()[iIndex].setDiffuseTextureWeight(4, m_iAltitudes_Sand.getValue(m_pMesh->vertices()[iIndex].altitude()) * dSlope);
                m_pMesh->vertices()[iIndex].setDiffuseTextureWeight(5, m_iAltitudes_Snow.getValue(m_pMesh->vertices()[iIndex].altitude()) * dSlope);

                m_pMesh->vertices()[iIndex].setDiffuseTextureWeight(6, m_iAltitudes_Grass.getValue(m_pMesh->vertices()[iIndex].altitude()) * dSlope * dGrassMix);
                m_pMesh->vertices()[iIndex].setDiffuseTextureWeight(7, m_iAltitudes_Grass.getValue(m_pMesh->vertices()[iIndex].altitude()) * dSlope * (1.0 - dGrassMix));
            }

            vVertexCount++;

            if (m_bStopRequested)
            {
                return;
            }
        }

        // Remove underwater if terrain is large
        if (m_bIsWater == false && m_iLevel >= m_iMaxLevel / 2)
        {
            for (int iFaceIndex = 0; iFaceIndex < m_pMesh->faces().count(); iFaceIndex++)
            {
                bool bIsOversea = false;

                for (int iVertexIndex = 0; iVertexIndex < m_pMesh->faces()[iFaceIndex].indices().count(); iVertexIndex++)
                {
                    if (m_pMesh->vertices()[m_pMesh->faces()[iFaceIndex].indices()[iVertexIndex]].altitude() >= 0.0)
                    {
                        bIsOversea = true;
                    }
                }

                if (bIsOversea == false)
                {
                    m_pMesh->faces().remove(iFaceIndex);
                    iFaceIndex--;
                }

                if (m_bStopRequested)
                {
                    return;
                }
            }
        }
    }

    // Update mesh
    m_pMesh->setGeometryDirty(true);

    // Terrain is ready
    m_bOK = true;

    /*
    LOG_METHOD_DEBUG(QString("Terrain built, vertices = %1, faces = %2")
        .arg(m_pMesh->getVertices().count())
        .arg(m_pMesh->getFaces().count())
        );
        */

    if (m_pMesh->faces().count() == 0)
    {
        LOG_METHOD_WARNING(QString("No faces in %1").arg(m_sName));
    }

    LOG_METHOD_DEBUG(QString("FINISHED : %1").arg(m_sName));
}

//-------------------------------------------------------------------------------------------------

void CTerrain::flatten(const CGeoloc& gPosition, double dRadius)
{
    if (worldBounds().contains(gPosition, dRadius))
    {
    }

    /*
    Vector3 vLocal = vPosition - m_vCenter;

    for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
    {
        double dDistance = (vLocal - m_vVertices[iIndex].position()).getMagnitude();

        if (dDistance < dRadius)
        {
            double dFactor = 1.0 - (dDistance / dRadius);

            Vector3 vNormal = vPosition.Normalize();

            m_vVertices[iIndex].position().Y += (vPosition.Y - m_vVertices[iIndex].position().Y) * dFactor;
        }
    }

    // Update mesh
    updateGeometry(true);
    */
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CTerrain::intersect(Math::CRay3 ray)
{
    if (m_pMesh != nullptr)
    {
        return m_pMesh->intersect(this, ray);
    }

    return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

void CTerrain::buildVerticesToFaceMap()
{
    m_mVerticesToFace.clear();

    if (m_pMesh != nullptr)
    {
        for (int iIndex = 0; iIndex < m_pMesh->faces().count(); iIndex++)
        {
            if (m_pMesh->faces()[iIndex].indices().count() == 4)
            {
                QString sKey = QString("%1%2%3%4")
                        .arg(m_pMesh->faces()[iIndex].indices()[0])
                        .arg(m_pMesh->faces()[iIndex].indices()[1])
                        .arg(m_pMesh->faces()[iIndex].indices()[2])
                        .arg(m_pMesh->faces()[iIndex].indices()[3])
                        ;

                m_mVerticesToFace[sKey] = iIndex;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CTerrain::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CTerrain]"));
    dumpIdent(stream, iIdent, QString("Original geoloc : %1").arg(m_gOriginalGeoloc.toString()));
    dumpIdent(stream, iIdent, QString("Original size : %1").arg(m_gOriginalSize.toString()));
    dumpIdent(stream, iIdent, QString("Size : %1").arg(m_gSize.toString()));
    dumpIdent(stream, iIdent, QString("Num. points : %1").arg(m_iNumPoints));
    dumpIdent(stream, iIdent, QString("Level : %1").arg(m_iLevel));
    dumpIdent(stream, iIdent, QString("Max level : %1").arg(m_iMaxLevel));
    dumpIdent(stream, iIdent, QString("Mesh :"));

    dumpOpenBlock(stream, iIdent); iIdent++;
    if (m_pMesh != nullptr) m_pMesh->dump(stream, iIdent);
    iIdent--; dumpCloseBlock(stream, iIdent);

    CComponent::dump(stream, iIdent);
}
