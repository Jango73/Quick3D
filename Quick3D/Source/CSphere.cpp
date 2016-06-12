
#include "CSphere.h"
#include "C3DScene.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CSphere::CSphere(C3DScene* pScene, double dMaxDistance, int iNumSegments)
    : CMesh(pScene, dMaxDistance)
{
    setName("Sphere");

    setMaterial(m_pScene->getRessourcesManager()->getDefaultMaterial());
    getMaterials()[0]->setIRFactor(0.4);

    createSphere(iNumSegments);

    setGeometryDirty(true);
}

//-------------------------------------------------------------------------------------------------

CSphere::~CSphere()
{
}
