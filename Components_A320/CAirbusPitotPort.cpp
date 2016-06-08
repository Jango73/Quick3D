
// Quick3D
#include "CLogManager.h"
#include "C3DScene.h"

#include "CAirbusPitotPort.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusPitotPort::instanciator(C3DScene* pScene)
{
    return new CAirbusPitotPort(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusPitotPort::CAirbusPitotPort(C3DScene* pScene)
    : CComponent(pScene)
{
    LOG_DEBUG("CAirbusPitotPort::CAirbusPitotPort()");
}

//-------------------------------------------------------------------------------------------------

CAirbusPitotPort::~CAirbusPitotPort()
{
    LOG_DEBUG("CAirbusPitotPort::~CAirbusPitotPort()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusPitotPort::update(double dDeltaTime)
{
}
