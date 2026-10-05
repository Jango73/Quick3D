
// qt-plus
#include "CLogger.h"

// Application
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CElectricalConsumer.h"

//-------------------------------------------------------------------------------------------------

CComponent* CElectricalConsumer::instantiator(C3DScene* pScene)
{
    return new CElectricalConsumer(pScene);
}

//-------------------------------------------------------------------------------------------------

CElectricalConsumer::CElectricalConsumer(C3DScene* pScene)
    : CElectricalComponent(pScene)
    , m_bPowered(false)
{
}

//-------------------------------------------------------------------------------------------------

CElectricalConsumer::~CElectricalConsumer()
{
}

//-------------------------------------------------------------------------------------------------

void CElectricalConsumer::update(double dDeltaTime)
{
    CElectricalComponent::update(dDeltaTime);

    m_tLoad = pull(m_tCurrent.m_dAmperage, dDeltaTime);

    m_bPowered =
            m_tLoad.m_dVoltage >= m_tCurrent.m_dVoltage * 0.8 &&
            m_tLoad.m_dAmperage > 0.0;
}
