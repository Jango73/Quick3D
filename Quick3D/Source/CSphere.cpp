
#include "CSphere.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CSphere::CSphere(C3DScene* pScene, double dMaxDistance, int iNumSegments)
: CMesh(pScene, dMaxDistance)
{
	setName("Sphere");

	getMaterials()[0]->setIRFactor(0.4);

	createSphere(iNumSegments);

	updateGeometry();
}

//-------------------------------------------------------------------------------------------------

CSphere::~CSphere()
{
}
