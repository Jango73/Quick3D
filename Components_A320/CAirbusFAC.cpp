
// Quick3D
#include "../Quick3D/CLogManager.h"
#include "../Quick3D/C3DScene.h"

// Application
#include "CAirbusFAC.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusFAC::instanciator(C3DScene* pScene)
{
	return new CAirbusFAC(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusFAC::CAirbusFAC(C3DScene* pScene)
: CAirbusFlightComputer(pScene)
{
	LOG_DEBUG("CAirbusFAC::CAirbusFAC()");
}

//-------------------------------------------------------------------------------------------------

CAirbusFAC::~CAirbusFAC()
{
	LOG_DEBUG("CAirbusFAC::~CAirbusFAC()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusFAC::update(double dDeltaTime)
{
	CAirbusFlightComputer::update(dDeltaTime);
}
