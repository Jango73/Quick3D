
// Application
#include "CConsoleBoard.h"
#include "CVector3.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CAircraftController.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAircraftController::instanciator(C3DScene* pScene)
{
    return new CAircraftController(pScene);
}

//-------------------------------------------------------------------------------------------------

CAircraftController::CAircraftController(C3DScene* pScene)
    : CStandardController(pScene)
    , m_bAileronLeft(false)
    , m_bAileronRight(false)
    , m_bNoseUp(false)
    , m_bNoseDown(false)
    , m_bRudderLeft(false)
    , m_bRudderRight(false)
{
}

//-------------------------------------------------------------------------------------------------

CAircraftController::~CAircraftController()
{
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::loadParameters(const QString& sBaseFile, CXMLNode xNode)
{
    CStandardController::loadParameters(sBaseFile, xNode);

    m_rLeftWingTarget.setName(xNode.attributes()["LeftWingTarget"]);
    m_rRightWingTarget.setName(xNode.attributes()["RightWingTarget"]);
    m_rElevatorTarget.setName(xNode.attributes()["ElevatorTarget"]);
    m_rRudderTarget.setName(xNode.attributes()["RudderTarget"]);
    m_rEngine1Target.setName(xNode.attributes()["Engine1Target"]);
    m_rEngine2Target.setName(xNode.attributes()["Engine2Target"]);
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::solveLinks(C3DScene* pScene)
{
    CStandardController::solveLinks(pScene);

    m_rLeftWingTarget.solve(pScene, QSP<CComponent>(this));
    m_rRightWingTarget.solve(pScene, QSP<CComponent>(this));
    m_rElevatorTarget.solve(pScene, QSP<CComponent>(this));
    m_rRudderTarget.solve(pScene, QSP<CComponent>(this));
    m_rEngine1Target.solve(pScene, QSP<CComponent>(this));
    m_rEngine2Target.solve(pScene, QSP<CComponent>(this));
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::clearLinks(C3DScene* pScene)
{
    CStandardController::clearLinks(pScene);

    m_rLeftWingTarget.clear();
    m_rRightWingTarget.clear();
    m_rElevatorTarget.clear();
    m_rRudderTarget.clear();
    m_rEngine1Target.clear();
    m_rEngine2Target.clear();
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::update(double dDeltaTime)
{
    CController::update(dDeltaTime);

    QSP<CWing> pLeftWing = QSP_CAST(CWing, m_rLeftWingTarget.component());
    QSP<CWing> pRightWing = QSP_CAST(CWing, m_rRightWingTarget.component());
    QSP<CElevator> pElevator = QSP_CAST(CElevator, m_rElevatorTarget.component());
    QSP<CRudder> pRudder = QSP_CAST(CRudder, m_rRudderTarget.component());

    if (m_pJoystick != nullptr && m_pJoystick->connected())
    {
        LOG_VALUE(QString("JOY X / Y / Z / R"),
                  QString("%1 / %2 / %3 / %4")
                  .arg(QString::number(m_pJoystick->axisStates()[0], 'f', 2))
                .arg(QString::number(m_pJoystick->axisStates()[1], 'f', 2))
                .arg(QString::number(m_pJoystick->axisStates()[2], 'f', 2))
                .arg(QString::number(m_pJoystick->axisStates()[3], 'f', 2))
                );

        if (pLeftWing && pRightWing && pElevator)
        {
            pLeftWing->setAileronAngle_norm(m_pJoystick->axisStates()[0]);
            pRightWing->setAileronAngle_norm(m_pJoystick->axisStates()[0] * -1.0);
        }

        if (pElevator != nullptr)
        {
            pElevator->setAileronAngle_norm(m_pJoystick->axisStates()[1]);
        }

        if (pRudder != nullptr)
        {
            pRudder->setAileronAngle_norm(m_pJoystick->axisStates()[3]);
        }

        QSP<CEngine> pEngine1 = QSP_CAST(CEngine, m_rEngine1Target.component());
        QSP<CEngine> pEngine2 = QSP_CAST(CEngine, m_rEngine2Target.component());

        double dAxis = 1.0 - ((m_pJoystick->axisStates()[2] + 1.0) * 0.5);

        if (pEngine1 != nullptr)
        {
            pEngine1->setCurrentFuelFlow_norm(dAxis);
        }

        if (pEngine2 != nullptr)
        {
            pEngine2->setCurrentFuelFlow_norm(dAxis);
        }
    }
    else
    {
        if (pLeftWing && pRightWing)
        {
            if (m_bAileronLeft)
            {
                pLeftWing->setAileronAngle_norm(-1.0);
                pRightWing->setAileronAngle_norm(1.0);
            }
            else
                if (m_bAileronRight)
                {
                    pLeftWing->setAileronAngle_norm(1.0);
                    pRightWing->setAileronAngle_norm(-1.0);
                }
                else
                {
                    pLeftWing->setAileronAngle_norm(0.0);
                    pRightWing->setAileronAngle_norm(0.0);
                }
        }

        if (pElevator != nullptr)
        {
            if (m_bNoseUp)
            {
                pElevator->setAileronAngle_norm(1.0);
            }
            else
                if (m_bNoseDown)
                {
                    pElevator->setAileronAngle_norm(-1.0);
                }
                else
                {
                    pElevator->setAileronAngle_norm(0.0);
                }
        }

        if (pRudder != nullptr)
        {
            if (m_bRudderLeft)
            {
                pRudder->setAileronAngle_norm(-1.0);
            }
            else
                if (m_bRudderRight)
                {
                    pRudder->setAileronAngle_norm(1.0);
                }
                else
                {
                    pRudder->setAileronAngle_norm(0.0);
                }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::keyPressEvent(QKeyEvent* event)
{
    CStandardController::keyPressEvent(event);

    QSP<CWing> pLeftWing = QSP_CAST(CWing, m_rLeftWingTarget.component());
    QSP<CWing> pRightWing = QSP_CAST(CWing, m_rRightWingTarget.component());
    QSP<CEngine> pEngine1 = QSP_CAST(CEngine, m_rEngine1Target.component());
    QSP<CEngine> pEngine2 = QSP_CAST(CEngine, m_rEngine2Target.component());

    switch (event->key())
    {
        case Qt::Key_D:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_AileronRight, CQ3DEvent::Press));
            break;
        case Qt::Key_Q:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_AileronLeft, CQ3DEvent::Press));
            break;
        case Qt::Key_S:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_NoseUp, CQ3DEvent::Press));
            break;
        case Qt::Key_Z:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_NoseDown, CQ3DEvent::Press));
            break;
        case Qt::Key_C:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_RudderRight, CQ3DEvent::Press));
            break;
        case Qt::Key_W:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_RudderLeft, CQ3DEvent::Press));
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
        case Qt::Key_PageUp:
        {
            if (pEngine1 != nullptr)
            {
                pEngine1->setCurrentFuelFlow_norm(pEngine1->currentFuelFlow_norm() + 0.1);
            }

            if (pEngine2 != nullptr)
            {
                pEngine2->setCurrentFuelFlow_norm(pEngine2->currentFuelFlow_norm() + 0.1);
            }
        }
            break;
        case Qt::Key_PageDown:
        {
            if (pEngine1 != nullptr)
            {
                pEngine1->setCurrentFuelFlow_norm(pEngine1->currentFuelFlow_norm() - 0.1);
            }

            if (pEngine2 != nullptr)
            {
                pEngine2->setCurrentFuelFlow_norm(pEngine2->currentFuelFlow_norm() - 0.1);
            }
        }
            break;
        case Qt::Key_M:
        {
            if (pLeftWing && pRightWing)
            {
                pLeftWing->setFlapsPosition_norm(pLeftWing->flapsPosition_norm() + 0.2);
                pRightWing->setFlapsPosition_norm(pRightWing->flapsPosition_norm() + 0.2);
            }
        }
            break;
        case Qt::Key_P:
        {
            if (pLeftWing && pRightWing)
            {
                pLeftWing->setFlapsPosition_norm(pLeftWing->flapsPosition_norm() - 0.2);
                pRightWing->setFlapsPosition_norm(pRightWing->flapsPosition_norm() - 0.2);
            }
        }
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::keyReleaseEvent(QKeyEvent *event)
{
    CStandardController::keyReleaseEvent(event);

    switch (event->key())
    {
        case Qt::Key_D:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_AileronRight, CQ3DEvent::Release));
            break;
        case Qt::Key_Q:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_AileronLeft, CQ3DEvent::Release));
            break;
        case Qt::Key_S:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_NoseUp, CQ3DEvent::Release));
            break;
        case Qt::Key_Z:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_NoseDown, CQ3DEvent::Release));
            break;
        case Qt::Key_C:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_RudderRight, CQ3DEvent::Release));
            break;
        case Qt::Key_W:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_RudderLeft, CQ3DEvent::Release));
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CAircraftController::q3dEvent(CQ3DEvent* event)
{
    CStandardController::q3dEvent(event);

    QSP<CComponent> pRotationTarget = m_rRotationTarget.component();

    if (event->getName() == Q3DEvent_AileronRight)
    {
        m_bAileronRight = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_AileronLeft)
    {
        m_bAileronLeft = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_NoseUp)
    {
        m_bNoseUp = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_NoseDown)
    {
        m_bNoseDown = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_RudderRight)
    {
        m_bRudderRight = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_RudderLeft)
    {
        m_bRudderLeft = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_LookFront)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3((Math::Pi / 20.0) * 1.0, (Math::Pi / 4.0) * 0.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookFrontRight)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 1.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookRight)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 2.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookBackRight)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 3.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookBack)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 4.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookBackLeft)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 5.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookLeft)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 6.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookFrontLeft)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3(0.0, (Math::Pi / 4.0) * 7.0, 0.0));
            }
        }
    }
    else if (event->getName() == Q3DEvent_LookFrontDown)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3((Math::Pi / 4.0) * 1.0, (Math::Pi / 4.0) * 1.0, 0.0));
            }
        }
    }
}
