
// Quick3D
#include "CLogManager.h"
#include "C3DScene.h"

// Application
#include "CAirbusFCU.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusFCU::instanciator(C3DScene* pScene)
{
    return new CAirbusFCU(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusFCU::CAirbusFCU(C3DScene* pScene)
    : CAirbusFlightComputer(pScene)
    , m_bAutoPilot1_Engaged(false)
    , m_bAutoPilot2_Engaged(false)
    , m_bAutoThrust_Engaged(false)
    , m_dSelectedHeading(0.0)
{
    LOG_DEBUG("CAirbusFCU::CAirbusFCU()");
}

//-------------------------------------------------------------------------------------------------

CAirbusFCU::~CAirbusFCU()
{
    LOG_DEBUG("CAirbusFCU::~CAirbusFCU()");
}

//-------------------------------------------------------------------------------------------------

void CAirbusFCU::update(double dDeltaTime)
{
    CAirbusFlightComputer::update(dDeltaTime);

    pushData(CAirbusData(m_sName, adFCU_AutoPilot1_Engaged, m_bAutoPilot1_Engaged));
    pushData(CAirbusData(m_sName, adFCU_AutoPilot2_Engaged, m_bAutoPilot2_Engaged));
    pushData(CAirbusData(m_sName, adFCU_AutoThrust_Engaged, m_bAutoThrust_Engaged));

    pushData(CAirbusData(m_sName, adFCU_Heading_deg, m_dSelectedHeading));
}

//-------------------------------------------------------------------------------------------------

void CAirbusFCU::toggle_AutoPilot1_Engaged()
{
    m_bAutoPilot1_Engaged = !m_bAutoPilot1_Engaged;
}

//-------------------------------------------------------------------------------------------------

void CAirbusFCU::toggle_AutoPilot2_Engaged()
{
    m_bAutoPilot2_Engaged = !m_bAutoPilot2_Engaged;
}

//-------------------------------------------------------------------------------------------------

void CAirbusFCU::toggle_AutoThrust_Engaged()
{
    m_bAutoThrust_Engaged = !m_bAutoThrust_Engaged;
}

//-------------------------------------------------------------------------------------------------

void CAirbusFCU::increment_SelectedHeading(bool bFast)
{
    m_dSelectedHeading++;

    m_dSelectedHeading = Math::Angles::clipAngleDegree(m_dSelectedHeading);
}

//-------------------------------------------------------------------------------------------------

void CAirbusFCU::decrement_SelectedHeading(bool bFast)
{
    m_dSelectedHeading--;

    m_dSelectedHeading = Math::Angles::clipAngleDegree(m_dSelectedHeading);
}
