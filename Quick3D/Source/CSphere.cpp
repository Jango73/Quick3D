
#include "CSphere.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CSphere::CSphere(C3DScene* pScene, double dMaxDistance, int iNumSegments)
    : CMesh(pScene, dMaxDistance)
{
    setName("Sphere");

    m_pGeometry->setMaterial(m_pScene->ressourcesManager()->getDefaultMaterial());
    m_pGeometry->materials()[0]->setIRFactor(0.4);

    m_pGeometry->createSphere(iNumSegments);
}

//-------------------------------------------------------------------------------------------------

CSphere::~CSphere()
{
}
