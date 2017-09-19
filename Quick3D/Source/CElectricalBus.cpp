
// qt-plus
#include "CLogger.h"

// Application
#include "C3DScene.h"
#include "CElectricalBus.h"

//-------------------------------------------------------------------------------------------------

CComponent* CElectricalBus::instantiator(C3DScene* pScene)
{
	return new CElectricalBus(pScene);
}

//-------------------------------------------------------------------------------------------------

CElectricalBus::CElectricalBus(C3DScene* pScene)
: CElectricalComponent(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CElectricalBus::~CElectricalBus()
{
}
