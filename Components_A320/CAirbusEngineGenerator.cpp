
// Quick3D
#include "CLogManager.h"
#include "C3DScene.h"

#include "CAirbusEngineGenerator.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusEngineGenerator::instanciator(C3DScene* pScene)
{
    return new CAirbusEngineGenerator(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusEngineGenerator::CAirbusEngineGenerator(C3DScene* pScene)
    : CElectricalComponent(pScene)
    , m_tCurrent(ctAC, 200.0, 150.0, 400.0)
{
    LOG_DEBUG("CAirbusEngineGenerator::CAirbusEngineGenerator()");
}

//-------------------------------------------------------------------------------------------------

CAirbusEngineGenerator::~CAirbusEngineGenerator()
{
    LOG_DEBUG("CAirbusEngineGenerator::~CAirbusEngineGenerator()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusEngineGenerator::update(double dDeltaTime)
{
    CElectricalComponent::update(dDeltaTime);

    push(m_tCurrent, dDeltaTime);
}
