
// Application
#include "CConsoleBoard.h"
#include "CVector3.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CAircraftController.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    \class CAircraftController
    \brief The base class for an aircraft controller.
    \inmodule Quick3D
    \sa CComponent
*/

//-------------------------------------------------------------------------------------------------

/*!
    Instantiates a new CAircraftController. \br\br
    \a pScene is the scene containing the component.
*/
CComponent* CAircraftController::instantiator(C3DScene* pScene)
{
    return new CAircraftController(pScene);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CAircraftController with its default parameters. \br\br
    \a pScene is the scene containing the component.
*/
CAircraftController::CAircraftController(C3DScene* pScene)
    : CStandardController(pScene)
    , m_bAileronLeft(false)
    , m_bAileronRight(false)
    , m_bNoseUp(false)
    , m_bNoseDown(false)
    , m_bRudderLeft(false)
    , m_bRudderRight(false)
    , m_bEngine1ThrustUp(false)
    , m_bEngine2ThrustUp(false)
    , m_bEngine1ThrustDown(false)
    , m_bEngine2ThrustDown(false)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CAircraftController.
*/
CAircraftController::~CAircraftController()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this component from \a xComponent. \br\br
    \a sBaseFile is the file name from which it is loaded.
*/
void CAircraftController::loadParameters(const QString& sBaseFile, const CXMLNode& xComponent)
{
    CStandardController::loadParameters(sBaseFile, xComponent);

    m_rLeftWingTarget.setName(xComponent.attributes()["LeftWingTarget"]);
    m_rRightWingTarget.setName(xComponent.attributes()["RightWingTarget"]);
    m_rElevatorTarget.setName(xComponent.attributes()["ElevatorTarget"]);
    m_rRudderTarget.setName(xComponent.attributes()["RudderTarget"]);
    m_rEngine1Target.setName(xComponent.attributes()[ParamName_Engine1Target]);
    m_rEngine2Target.setName(xComponent.attributes()[ParamName_Engine2Target]);
    m_rEngine3Target.setName(xComponent.attributes()[ParamName_Engine3Target]);
    m_rEngine4Target.setName(xComponent.attributes()[ParamName_Engine4Target]);
}

//-------------------------------------------------------------------------------------------------

/*!
    Solves the links in this component after it has been loaded from an object file (XML or JSON). \br\br
    \a pScene is the scene containing this component.
*/
void CAircraftController::solveLinks(C3DScene* pScene)
{
    CStandardController::solveLinks(pScene);

    m_rLeftWingTarget.solve(pScene, QSP<CComponent>(this));
    m_rRightWingTarget.solve(pScene, QSP<CComponent>(this));
    m_rElevatorTarget.solve(pScene, QSP<CComponent>(this));
    m_rRudderTarget.solve(pScene, QSP<CComponent>(this));
    m_rEngine1Target.solve(pScene, QSP<CComponent>(this));
    m_rEngine2Target.solve(pScene, QSP<CComponent>(this));
    m_rEngine3Target.solve(pScene, QSP<CComponent>(this));
    m_rEngine4Target.solve(pScene, QSP<CComponent>(this));
}

//-------------------------------------------------------------------------------------------------

/*!
    Clears the links in this component and its children. \br\br
    \a pScene is the scene containing this component.
*/
void CAircraftController::clearLinks(C3DScene* pScene)
{
    CStandardController::clearLinks(pScene);

    m_rLeftWingTarget.clear();
    m_rRightWingTarget.clear();
    m_rElevatorTarget.clear();
    m_rRudderTarget.clear();
    m_rEngine1Target.clear();
    m_rEngine2Target.clear();
    m_rEngine3Target.clear();
    m_rEngine4Target.clear();
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates this component using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CAircraftController::update(double dDeltaTimeS)
{
    CController::update(dDeltaTimeS);

    QSP<CWing> pLeftWing = QSP_CAST(CWing, m_rLeftWingTarget.component());
    QSP<CWing> pRightWing = QSP_CAST(CWing, m_rRightWingTarget.component());
    QSP<CElevator> pElevator = QSP_CAST(CElevator, m_rElevatorTarget.component());
    QSP<CRudder> pRudder = QSP_CAST(CRudder, m_rRudderTarget.component());
    QSP<CEngine> pEngine1 = QSP_CAST(CEngine, m_rEngine1Target.component());
    QSP<CEngine> pEngine2 = QSP_CAST(CEngine, m_rEngine2Target.component());

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
            else if (m_bAileronRight)
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
            else if (m_bNoseDown)
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
            else if (m_bRudderRight)
            {
                pRudder->setAileronAngle_norm(1.0);
            }
            else
            {
                pRudder->setAileronAngle_norm(0.0);
            }
        }

        if (pEngine1 != nullptr)
        {
            if (m_bEngine1ThrustUp)
            {
                pEngine1->setCurrentFuelFlow_norm(pEngine1->currentFuelFlow_norm() + 0.5 * dDeltaTimeS);
            }
            else if (m_bEngine1ThrustDown)
            {
                pEngine1->setCurrentFuelFlow_norm(pEngine1->currentFuelFlow_norm() - 0.5 * dDeltaTimeS);
            }
        }

        if (pEngine2 != nullptr)
        {
            if (m_bEngine2ThrustUp)
            {
                pEngine2->setCurrentFuelFlow_norm(pEngine2->currentFuelFlow_norm() + 0.5 * dDeltaTimeS);
            }
            else if (m_bEngine2ThrustDown)
            {
                pEngine2->setCurrentFuelFlow_norm(pEngine2->currentFuelFlow_norm() - 0.5 * dDeltaTimeS);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Handles the key press \a event.
*/
void CAircraftController::keyPressEvent(QKeyEvent* event)
{
    CStandardController::keyPressEvent(event);

    QSP<CWing> pLeftWing = QSP_CAST(CWing, m_rLeftWingTarget.component());
    QSP<CWing> pRightWing = QSP_CAST(CWing, m_rRightWingTarget.component());

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

        case Qt::Key_PageUp:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Engine1ThrustUp, CQ3DEvent::Press));
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Engine2ThrustUp, CQ3DEvent::Press));
            break;

        case Qt::Key_PageDown:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Engine1ThrustDown, CQ3DEvent::Press));
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Engine2ThrustDown, CQ3DEvent::Press));
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

        case Qt::Key_Dollar:
            if (pLeftWing && pRightWing)
            {
                pLeftWing->setFlapsPosition_norm(pLeftWing->flapsPosition_norm() + 0.2);
                pRightWing->setFlapsPosition_norm(pRightWing->flapsPosition_norm() + 0.2);
            }
            break;

        case Qt::Key_Asterisk:
            if (pLeftWing && pRightWing)
            {
                pLeftWing->setFlapsPosition_norm(pLeftWing->flapsPosition_norm() - 0.2);
                pRightWing->setFlapsPosition_norm(pRightWing->flapsPosition_norm() - 0.2);
            }
            break;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Handles the key release \a event.
*/
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

        case Qt::Key_PageUp:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Engine1ThrustUp, CQ3DEvent::Release));
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Engine2ThrustUp, CQ3DEvent::Release));
            break;

        case Qt::Key_PageDown:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Engine1ThrustDown, CQ3DEvent::Release));
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Engine2ThrustDown, CQ3DEvent::Release));
            break;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Handles the Quick3D \a event.
*/
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
    else if (event->getName() == Q3DEvent_Engine1ThrustUp)
    {
        m_bEngine1ThrustUp = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_Engine2ThrustUp)
    {
        m_bEngine2ThrustUp = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_Engine1ThrustDown)
    {
        m_bEngine1ThrustDown = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_Engine2ThrustDown)
    {
        m_bEngine2ThrustDown = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_LookFront)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3(0.0, 0.0, 0.0));
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
