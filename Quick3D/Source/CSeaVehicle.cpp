
// Application
#include "C3DScene.h"
#include "CSeaVehicle.h"

//-------------------------------------------------------------------------------------------------

CComponent* CSeaVehicle::instantiator(C3DScene* pScene)
{
    return new CSeaVehicle(pScene);
}

//-------------------------------------------------------------------------------------------------

CSeaVehicle::CSeaVehicle(C3DScene* pScene)
    : CVehicle(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CSeaVehicle::~CSeaVehicle()
{
}
