
// Application
#include "C3DScene.h"
#include "CWater.h"
#include "CWaterMaterial.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CWater::CWater(C3DScene* pScene, double dSize, int iIterations)
    : CMesh(pScene)
    , CHeightField(0.4)
{
    m_pGeometry->setMaterial(m_pScene->ressourcesManager()->getWaterMaterial());

    m_pGeometry->createCircularQuadPatch(CVector3(), iIterations);

    for (int iIndex = 0; iIndex < m_pGeometry->vertices().count(); iIndex++)
    {
        m_pGeometry->vertices()[iIndex].position() = m_pGeometry->vertices()[iIndex].position() * dSize;
    }

    m_pGeometry->setGeometryDirty(true);
}

//-------------------------------------------------------------------------------------------------

CWater::~CWater()
{
}

//-------------------------------------------------------------------------------------------------

void CWater::update(double dDeltaTime)
{
    CMesh::update(dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CWater::paint(CRenderContext* pContext)
{
    CGeoloc gPosition = pContext->camera()->geoloc();

    if (gPosition.Altitude > 2000.0) return;

    setGeoloc(CGeoloc(gPosition.Latitude, gPosition.Longitude, 0.0));
    setRotation(CVector3(0.0, pContext->camera()->rotation().Y, 0.0));

    computeWorldTransform();

    // double dScale = fabs(pContext->scene()->getCamera()->getWorldPosition().Y) / 80.0;
    // if (dScale < 1.0) dScale = 1.0;
    // setOriginScale(Vector3(dScale, dScale, dScale));

    CMesh::paint(pContext);
}

//-------------------------------------------------------------------------------------------------

double CWater::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (pRigidness != nullptr) *pRigidness = 0.0;

    if (m_pGeometry->materials().count() > 0)
    {
        CWaterMaterial* pMat = dynamic_cast<CWaterMaterial*>(m_pGeometry->materials()[0].data());

        if (pRigidness != nullptr) *pRigidness = 0.25;

        return pMat->WaveHeight(gPosition);
    }

    return Q3D_INFINITY;
}
