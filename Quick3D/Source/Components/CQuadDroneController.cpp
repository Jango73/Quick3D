
// qt-plus
#include "CAverager.h"

// Application
#include "CConsoleBoard.h"
#include "CVector3.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CQuadDroneController.h"

using namespace Math;

/*-------------------------------------------------------------------------------------------------

Engine numbers (top view) :

    1   2
      O
    3   4

-------------------------------------------------------------------------------------------------*/

CComponent* CQuadDroneController::instantiator(C3DScene* pScene)
{
    return new CQuadDroneController(pScene);
}

//-------------------------------------------------------------------------------------------------

CQuadDroneController::CQuadDroneController(C3DScene* pScene)
    : CAircraftController(pScene)
    , m_dMainThrust(0.0)
{
    m_iThrustModifiers_Forward.addValue(-1.0, CVector4(0.0, 0.0, 1.0, 1.0));
    m_iThrustModifiers_Forward.addValue( 0.0, CVector4(0.5, 0.5, 0.5, 0.5));
    m_iThrustModifiers_Forward.addValue( 1.0, CVector4(1.0, 1.0, 0.0, 0.0));

    m_iThrustModifiers_Lateral.addValue(-1.0, CVector4(0.0, 1.0, 0.0, 1.0));
    m_iThrustModifiers_Lateral.addValue( 0.0, CVector4(0.5, 0.5, 0.5, 0.5));
    m_iThrustModifiers_Lateral.addValue( 1.0, CVector4(1.0, 0.0, 1.0, 0.0));

    m_iThrustModifiers_Yaw.addValue(-1.0, CVector4(1.0, 0.0, 0.0, 1.0));
    m_iThrustModifiers_Yaw.addValue( 0.0, CVector4(0.5, 0.5, 0.5, 0.5));
    m_iThrustModifiers_Yaw.addValue( 1.0, CVector4(0.0, 1.0, 1.0, 0.0));
}

//-------------------------------------------------------------------------------------------------

CQuadDroneController::~CQuadDroneController()
{
}

//-------------------------------------------------------------------------------------------------

void CQuadDroneController::update(double dDeltaTimeS)
{
    CController::update(dDeltaTimeS);

    QSP<CEngine> pEngine1 = QSP_CAST(CEngine, m_rEngine1Target.component());
    QSP<CEngine> pEngine2 = QSP_CAST(CEngine, m_rEngine2Target.component());
    QSP<CEngine> pEngine3 = QSP_CAST(CEngine, m_rEngine3Target.component());
    QSP<CEngine> pEngine4 = QSP_CAST(CEngine, m_rEngine4Target.component());

    if (pEngine1 != nullptr && pEngine2 != nullptr && pEngine3 != nullptr && pEngine4 != nullptr)
    {
        double dForwardDemand = 0.0;
        double dLateralDemand = 0.0;
        double dVertSpeedDemand = 0.0;
        double dYawDemand = 0.0;

        if (m_pJoystick != nullptr && m_pJoystick->connected())
        {
            LOG_VALUE(QString("JOY X / Y / Z / R"),
                      QString("%1 / %2 / %3 / %4")
                      .arg(QString::number(m_pJoystick->axisStates()[0], 'f', 2))
                    .arg(QString::number(m_pJoystick->axisStates()[1], 'f', 2))
                    .arg(QString::number(m_pJoystick->axisStates()[2], 'f', 2))
                    .arg(QString::number(m_pJoystick->axisStates()[3], 'f', 2))
                    );

            dLateralDemand = m_pJoystick->axisStates()[0];
            dForwardDemand = m_pJoystick->axisStates()[1];
            dYawDemand = m_pJoystick->axisStates()[3];
            dVertSpeedDemand = 1.0 - ((m_pJoystick->axisStates()[2] + 1.0) * 0.5);
        }
        else
        {
            if (m_bNoseUp)
                dForwardDemand = -1.0;
            else if (m_bNoseDown)
                dForwardDemand = 1.0;

            if (m_bAileronRight)
                dLateralDemand = 1.0;
            else if (m_bAileronLeft)
                dLateralDemand = -1.0;

            if (m_bRudderRight)
                dYawDemand = 1.0;
            else if (m_bRudderLeft)
                dYawDemand = -1.0;

            if (m_bEngine1ThrustUp)
                m_dMainThrust += 0.25 * dDeltaTimeS;
            else if (m_bEngine1ThrustDown)
                m_dMainThrust -= 0.25 * dDeltaTimeS;

            m_dMainThrust = Angles::clipDouble(m_dMainThrust, -1.0, 1.0);
        }

        CAverager<CVector4> aEngineThrustModifiers(3);

        aEngineThrustModifiers << m_iThrustModifiers_Forward.getValue(dForwardDemand);
        aEngineThrustModifiers << m_iThrustModifiers_Lateral.getValue(dLateralDemand);
        aEngineThrustModifiers << m_iThrustModifiers_Yaw.getValue(dYawDemand);

        CVector4 vFinalThrust(m_dMainThrust);
        vFinalThrust += aEngineThrustModifiers.getAverage();

        LOG_VALUE("vFinalThrust", vFinalThrust.toString());

        pEngine1->setCurrentFuelFlow_norm(vFinalThrust.X);
        pEngine2->setCurrentFuelFlow_norm(vFinalThrust.Y);
        pEngine3->setCurrentFuelFlow_norm(vFinalThrust.Z);
        pEngine4->setCurrentFuelFlow_norm(vFinalThrust.W);
    }
}
