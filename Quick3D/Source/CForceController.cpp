
// Application
#include "CVector3.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CForceController.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

#define TURN_SPEED	40.0

//-------------------------------------------------------------------------------------------------

CComponent* CForceController::instanciator(C3DScene* pScene)
{
    return new CForceController(pScene);
}

//-------------------------------------------------------------------------------------------------

CForceController::CForceController(C3DScene* pScene)
    : CStandardController(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CForceController::~CForceController()
{
}

//-------------------------------------------------------------------------------------------------

void CForceController::update(double dDeltaTime)
{
    CController::update(dDeltaTime);

    double dMoveSpeed = 1.5 + (m_dMoveSpeed * m_dForceFactor * 300.0);

    if (m_rPositionTarget.component())
    {
        QSP<CPhysicalComponent> pPhysicalPositionTarget = QSP_CAST(CPhysicalComponent, m_rPositionTarget.component());

        if (pPhysicalPositionTarget != NULL)
        {
            double dMoveFactor = pPhysicalPositionTarget->getGeoloc().Altitude / 500.0;
            if (dMoveFactor < 1.0) dMoveFactor = 1.0;

            dMoveSpeed = dMoveSpeed * dMoveFactor;

            if (m_pJoystick != NULL && m_pJoystick->connected())
            {
                pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, 0.0, m_pJoystick->getAxisStates()[1] * -dMoveSpeed));
            }
            else
            {
                if (m_bGoForward)
                {
                    pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, 0.0, dMoveSpeed));
                }
                else
                    if (m_bGoBackward)
                    {
                        pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, 0.0, -dMoveSpeed));
                    }
            }

            if (m_bGoUp)
            {
                pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, dMoveSpeed, 0.0));
            }
            else
                if (m_bGoDown)
                {
                    pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, -dMoveSpeed, 0.0));
                }

            if (m_bStrafeRight)
            {
                pPhysicalPositionTarget->addLocalForce_kg(CVector3(dMoveSpeed, 0.0, 0.0));
            }
            else
                if (m_bStrafeLeft)
                {
                    pPhysicalPositionTarget->addLocalForce_kg(CVector3(-dMoveSpeed, 0.0, 0.0));
                }
        }
    }

    if (m_rRotationTarget.component())
    {
        QSP<CPhysicalComponent> pPhysicalRotationTarget = QSP_CAST(CPhysicalComponent, m_rRotationTarget.component());

        if (pPhysicalRotationTarget)
        {
            double dRotationSpeed = m_dMoveSpeed * m_dForceFactor * 50.0;

            if (m_bLookUp)
            {
                CVector3 vRotation = CVector3(Math::Angles::toRad(TURN_SPEED) * dDeltaTime, 0.0, 0.0);

                pPhysicalRotationTarget->setOriginRotation(m_rRotationTarget.component()->getOriginRotation() + vRotation);
            }
            else
                if (m_bLookDown)
                {
                    CVector3 vRotation = CVector3(Math::Angles::toRad(-TURN_SPEED) * dDeltaTime, 0.0, 0.0);

                    pPhysicalRotationTarget->setOriginRotation(m_rRotationTarget.component()->getOriginRotation() + vRotation);
                }

            if (m_pJoystick != NULL && m_pJoystick->connected())
            {
                pPhysicalRotationTarget->addLocalTorque_kg(CVector3(0.0, m_pJoystick->getAxisStates()[0] * dRotationSpeed, 0.0));
            }
            else
            {
                if (m_bTurnRight)
                {
                    if (pPhysicalRotationTarget->isRootObject())
                    {
                        pPhysicalRotationTarget->addLocalTorque_kg(CVector3(0.0, dRotationSpeed, 0.0));
                    }
                    else
                    {
                        CVector3 vRotation = CVector3(0.0, Math::Angles::toRad(TURN_SPEED) * dDeltaTime, 0.0);

                        pPhysicalRotationTarget->setOriginRotation(m_rRotationTarget.component()->getOriginRotation() + vRotation);
                    }
                }
                else
                    if (m_bTurnLeft)
                    {
                        if (pPhysicalRotationTarget->isRootObject())
                        {
                            pPhysicalRotationTarget->addLocalTorque_kg(CVector3(0.0, -dRotationSpeed, 0.0));
                        }
                        else
                        {
                            CVector3 vRotation = CVector3(0.0, Math::Angles::toRad(-TURN_SPEED) * dDeltaTime, 0.0);

                            pPhysicalRotationTarget->setOriginRotation(m_rRotationTarget.component()->getOriginRotation() + vRotation);
                        }
                    }
            }
        }
    }
}
