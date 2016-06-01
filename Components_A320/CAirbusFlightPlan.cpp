
// Quick3D
#include "../Quick3D/CLogManager.h"
#include "../Quick3D/C3DScene.h"

// Application
#include "CAirbusFlightPlan.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CAirbusFlightPlan::CAirbusFlightPlan()
	: m_iCurrentWaypoint(1)
{
	LOG_DEBUG("CAirbusFlightPlan::CAirbusFlightPlan()");
}

//-------------------------------------------------------------------------------------------------

CAirbusFlightPlan::~CAirbusFlightPlan()
{
	LOG_DEBUG("CAirbusFlightPlan::~CAirbusFlightPlan()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusFlightPlan::update(double dDeltaTime)
{
}

//-------------------------------------------------------------------------------------------------

void CAirbusFlightPlan::nextWaypoint()
{
	m_iCurrentWaypoint++;
}
