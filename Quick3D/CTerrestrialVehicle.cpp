
// Application
#include "C3DScene.h"
#include "CTerrestrialVehicle.h"

//-------------------------------------------------------------------------------------------------

CComponent* CTerrestrialVehicle::instanciator(C3DScene* pScene)
{
	return new CTerrestrialVehicle(pScene);
}

//-------------------------------------------------------------------------------------------------

CTerrestrialVehicle::CTerrestrialVehicle(C3DScene* pScene)
: CVehicle(pScene)
{
	LOG_DEBUG("CTerrestrialVehicle::CTerrestrialVehicle()");
}

//-------------------------------------------------------------------------------------------------

CTerrestrialVehicle::~CTerrestrialVehicle()
{
	LOG_DEBUG("CTerrestrialVehicle::~CTerrestrialVehicle()");
}

//-------------------------------------------------------------------------------------------------

void CTerrestrialVehicle::loadParameters(CXMLNode xComponent)
{
	CVehicle::loadParameters(xComponent);
}
