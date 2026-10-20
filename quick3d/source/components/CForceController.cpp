
// qt-plus
#include "CLogger.h"

// STL
#include <cmath>

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

CComponent* CForceController::instantiator(C3DScene* pScene)
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

    double dMoveSpeed = m_dForceFactor;

    if (m_rPositionTarget.component())
    {
        QSP<CPhysicalComponent> pPhysicalPositionTarget = QSP_CAST(CPhysicalComponent, m_rPositionTarget.component());

        if (pPhysicalPositionTarget != nullptr)
        {
            if (pPhysicalPositionTarget->isDriveable() == true && pPhysicalPositionTarget->hasPhysicsBody() == true)
            {
                double dHandBrakeNorm = pPhysicalPositionTarget->isHandBrakeEngaged() ? 1.0 : 0.0;
                double dForwardNorm = driveForwardNorm();
                double dBrakeNorm = 0.0;

                // Note: heavy vehicles carry high torque for grades, the
                // authored speed caps the top speed instead, throttle tapers
                // to zero over the last 1 m/s before the cap and a staged
                // brake holds the vehicle back past it, reverse stays free so
                // no binary throttle brake jerk can build up into a weave
                double dSpeedLimitMS = pPhysicalPositionTarget->driveSpeedLimitMS();
                Math::CVector3 vVelocity = pPhysicalPositionTarget->velocity_ms();
                double dHorizontalMS = sqrt(vVelocity.X * vVelocity.X + vVelocity.Z * vVelocity.Z);

                if (dSpeedLimitMS > 0.0)
                {
                    if (dForwardNorm > 0.0)
                    {
                        dForwardNorm *= qBound(0.0, (dSpeedLimitMS - dHorizontalMS) / 1.0, 1.0);
                    }

                    if (dHorizontalMS > dSpeedLimitMS + 0.5)
                    {
                        dBrakeNorm = 0.4;
                    }
                    else if (dHorizontalMS > dSpeedLimitMS)
                    {
                        dBrakeNorm = 0.15;
                    }
                }

                QString sDriverInputLog = QString("%1 fwd=%2 right=%3 brake=%4 hand=%5 speed=%6 limit=%7")
                    .arg(pPhysicalPositionTarget->name())
                    .arg(dForwardNorm, 0, 'f', 2).arg(driveRightNorm(), 0, 'f', 2)
                    .arg(dBrakeNorm, 0, 'f', 2).arg(dHandBrakeNorm, 0, 'f', 2)
                    .arg(dHorizontalMS, 0, 'f', 2).arg(dSpeedLimitMS, 0, 'f', 2);

                if (m_cDriverInputLog.hasChanged(sDriverInputLog) == true)
                {
                    LOG_DEBUG(sDriverInputLog);
                }

                pPhysicalPositionTarget->setDriverInputNorm(dForwardNorm, driveRightNorm(), dBrakeNorm, dHandBrakeNorm);
            }
            else
            {
                double dMoveFactor = pPhysicalPositionTarget->geoloc().Altitude / 500.0;
                if (dMoveFactor < 1.0) dMoveFactor = 1.0;

                dMoveSpeed = dMoveSpeed * dMoveFactor;

                if (m_pJoystick != nullptr && m_pJoystick->connected())
                {
                    pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, 0.0, m_pJoystick->axisStates()[1] * -dMoveSpeed));
                }
                else
                {
                    if (m_bGoForward)
                    {
                        pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, 0.0, dMoveSpeed));
                    }
                    else if (m_bGoBackward)
                    {
                        pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, 0.0, -dMoveSpeed));
                    }
                }

                if (m_bGoUp)
                {
                    pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, dMoveSpeed, 0.0));
                }
                else if (m_bGoDown)
                {
                    pPhysicalPositionTarget->addLocalForce_kg(CVector3(0.0, -dMoveSpeed, 0.0));
                }

                if (m_bStrafeRight)
                {
                    pPhysicalPositionTarget->addLocalForce_kg(CVector3(dMoveSpeed, 0.0, 0.0));
                }
                else if (m_bStrafeLeft)
                {
                    pPhysicalPositionTarget->addLocalForce_kg(CVector3(-dMoveSpeed, 0.0, 0.0));
                }
            }
        }
    }

    if (m_rRotationTarget.component())
    {
        QSP<CPhysicalComponent> pPhysicalRotationTarget = QSP_CAST(CPhysicalComponent, m_rRotationTarget.component());

        if (pPhysicalRotationTarget != nullptr)
        {
            // Note: a physics attached chassis steers through the wheel driver
            // inputs above, direct rotation writes fight the constraint solver
            // and eject the body, only detached targets keep arcade rotation
            bool bPhysicsDriven = pPhysicalRotationTarget->isDriveable() == true && pPhysicalRotationTarget->hasPhysicsBody() == true;

            // Note: a physics attached chassis steers through the wheel driver
            // inputs above, direct rotation writes fight the constraint solver
            // and eject the body, only detached targets keep arcade rotation
            if (bPhysicsDriven == true)
            {
                return;
            }

            double dRotationSpeed = m_dForceFactor * 0.02;

            if (m_bLookUp)
            {
                CVector3 vRotation = CVector3(Math::Angles::toRad(TURN_SPEED) * dDeltaTime, 0.0, 0.0);

                pPhysicalRotationTarget->setRotation(m_rRotationTarget.component()->rotation() + vRotation);
            }
            else if (m_bLookDown)
            {
                CVector3 vRotation = CVector3(Math::Angles::toRad(-TURN_SPEED) * dDeltaTime, 0.0, 0.0);

                pPhysicalRotationTarget->setRotation(m_rRotationTarget.component()->rotation() + vRotation);
            }

            if (m_pJoystick != nullptr && m_pJoystick->connected())
            {
                pPhysicalRotationTarget->addLocalTorque_kg(CVector3(0.0, m_pJoystick->axisStates()[0] * dRotationSpeed, 0.0));
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

                        pPhysicalRotationTarget->setRotation(m_rRotationTarget.component()->rotation() + vRotation);
                    }
                }
                else if (m_bTurnLeft)
                {
                    if (pPhysicalRotationTarget->isRootObject())
                    {
                        pPhysicalRotationTarget->addLocalTorque_kg(CVector3(0.0, -dRotationSpeed, 0.0));
                    }
                    else
                    {
                        CVector3 vRotation = CVector3(0.0, Math::Angles::toRad(-TURN_SPEED) * dDeltaTime, 0.0);

                        pPhysicalRotationTarget->setRotation(m_rRotationTarget.component()->rotation() + vRotation);
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CForceController::keyPressEvent(QKeyEvent* event)
{
    CStandardController::keyPressEvent(event);

    switch (event->key())
    {
        case Qt::Key_H:
            if (m_rPositionTarget.component())
            {
                QSP<CPhysicalComponent> pTargetVehicle = QSP_CAST(CPhysicalComponent, m_rPositionTarget.component());

                if (pTargetVehicle != nullptr && pTargetVehicle->isDriveable() == true)
                {
                    pTargetVehicle->setHandBrakeEngaged(pTargetVehicle->isHandBrakeEngaged() == false);

                    LOG_METHOD_INFO(QString("Hand brake of %1 engaged: %2").arg(pTargetVehicle->name()).arg(pTargetVehicle->isHandBrakeEngaged()));
                }
            }
            break;
        case Qt::Key_8:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookFront, CQ3DEvent::Press));
            break;
        case Qt::Key_9:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookFrontRight, CQ3DEvent::Press));
            break;
        case Qt::Key_6:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookRight, CQ3DEvent::Press));
            break;
        case Qt::Key_3:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookBackRight, CQ3DEvent::Press));
            break;
        case Qt::Key_2:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookBack, CQ3DEvent::Press));
            break;
        case Qt::Key_1:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookBackLeft, CQ3DEvent::Press));
            break;
        case Qt::Key_4:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookLeft, CQ3DEvent::Press));
            break;
        case Qt::Key_7:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookFrontLeft, CQ3DEvent::Press));
            break;
        case Qt::Key_5:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookFrontDown, CQ3DEvent::Press));
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CForceController::keyReleaseEvent(QKeyEvent *event)
{
    CStandardController::keyReleaseEvent(event);
}

//-------------------------------------------------------------------------------------------------

void CForceController::q3dEvent(CQ3DEvent* event)
{
    CStandardController::q3dEvent(event);

    QSP<CComponent> pLookTarget = m_rLookTarget.component();

    if (event->getName() == Q3DEvent_LookFront)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pLookTarget != nullptr)
            {
                pLookTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 0.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookFrontRight)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pLookTarget != nullptr)
            {
                pLookTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 1.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookRight)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pLookTarget != nullptr)
            {
                pLookTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 2.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookBackRight)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pLookTarget != nullptr)
            {
                pLookTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 3.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookBack)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pLookTarget != nullptr)
            {
                pLookTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 4.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookBackLeft)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pLookTarget != nullptr)
            {
                pLookTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 5.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookLeft)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pLookTarget != nullptr)
            {
                pLookTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 6.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookFrontLeft)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pLookTarget != nullptr)
            {
                pLookTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 7.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookFrontDown)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pLookTarget != nullptr)
            {
                pLookTarget->setRotation(CVector3((Math::Pi / 4.0) * 1.0, (Math::Pi / 4.0) * 1.0, 0.0));
            }
        }
    }
}
