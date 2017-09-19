
// Application
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CJetEngine.h"

//-------------------------------------------------------------------------------------------------

CComponent* CJetEngine::instantiator(C3DScene* pScene)
{
    return new CJetEngine(pScene);
}

//-------------------------------------------------------------------------------------------------

CJetEngine::CJetEngine(C3DScene* pScene)
    : CEngine(pScene)
    , m_dN1_servo(0.5, 0.5, 0.5, 0.2)
    , m_dN2_norm(0.0)
{
}

//-------------------------------------------------------------------------------------------------

CJetEngine::~CJetEngine()
{
}

//-------------------------------------------------------------------------------------------------

double CJetEngine::currentThrust_kg() const
{
    // We just take into account N1 that is above 0.5, because 0.5 is idle N1 and below that no thrust is produced
    double dFactor = Math::Angles::clipDouble((m_dN1_servo.position() - 0.5) * 2.0, 0.0, 1.0);
    return dFactor * m_dMaxThrust_kg;
}

//-------------------------------------------------------------------------------------------------

double CJetEngine::currentFuelCons_ls() const
{
    return m_dFuelFlow_norm * 0.01;
}

//-------------------------------------------------------------------------------------------------

bool CJetEngine::alternatorActive() const
{
    return m_dN1_servo.position() > 0.1;
}

//-------------------------------------------------------------------------------------------------

double CJetEngine::n1_norm() const
{
    return m_dN1_servo.position();
}

//-------------------------------------------------------------------------------------------------

double CJetEngine::n2_norm() const
{
    return m_dN2_norm;
}

//-------------------------------------------------------------------------------------------------

void CJetEngine::update(double dDeltaTime)
{
    m_dN1_servo.setTargetPosition(0.5 + (m_dFuelFlow_norm / 2.0));
    m_dN1_servo.update(dDeltaTime);

    LOG_VALUE(QString("%1 FF / N1 / THT KG").arg(m_sName),
              QString("%1 / %2 / %3")
              .arg(QString::number(m_dFuelFlow_norm, 'f', 4))
              .arg(QString::number(m_dN1_servo.position(), 'f', 4))
              .arg(QString::number(currentThrust_kg(), 'f', 2))
              );

    CEngine::update(dDeltaTime);
}
