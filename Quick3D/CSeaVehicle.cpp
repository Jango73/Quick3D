
// Application
#include "C3DScene.h"
#include "CSeaVehicle.h"

//-------------------------------------------------------------------------------------------------

CComponent* CSeaVehicle::instanciator(C3DScene* pScene)
{
	return new CSeaVehicle(pScene);
}

//-------------------------------------------------------------------------------------------------

CSeaVehicle::CSeaVehicle(C3DScene* pScene)
: CVehicle(pScene)
{
	LOG_DEBUG("CSeaVehicle::CSeaVehicle()");
}

//-------------------------------------------------------------------------------------------------

CSeaVehicle::~CSeaVehicle()
{
	LOG_DEBUG("CSeaVehicle::~CSeaVehicle()");
}
