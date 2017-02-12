
// Quick3D
#include "CConsoleBoard.h"

// Application
#include "CAirbusController.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CAirbusController::instanciator(C3DScene* pScene)
{
    return new CAirbusController(pScene);
}

//-------------------------------------------------------------------------------------------------

CAirbusController::CAirbusController(C3DScene* pScene)
    : CAircraftController(pScene)
{
}

//-------------------------------------------------------------------------------------------------

CAirbusController::~CAirbusController()
{
}

//-------------------------------------------------------------------------------------------------

void CAirbusController::loadParameters(const QString& sBaseFile, CXMLNode xNode)
{
    CAircraftController::loadParameters(sBaseFile, xNode);

    m_rFCU.setName(xNode.attributes()["FCU"]);
}

//-------------------------------------------------------------------------------------------------

void CAirbusController::solveLinks(C3DScene* pScene)
{
    CAircraftController::solveLinks(pScene);

    m_rFCU.solve(pScene, QSP<CComponent>(this));
}

//-------------------------------------------------------------------------------------------------

void CAirbusController::clearLinks(C3DScene* pScene)
{
    CAircraftController::clearLinks(pScene);

    m_rFCU.clear();
}

//-------------------------------------------------------------------------------------------------

void CAirbusController::update(double dDeltaTime)
{
    CController::update(dDeltaTime);

    if (m_pJoystick != NULL && m_pJoystick->connected())
    {
        LOG_VALUE(QString("JOY X / Y / Z / R"),
                  QString("%1 / %2 / %3 / %4")
                  .arg(QString::number(m_pJoystick->axisStates()[0], 'f', 2))
                .arg(QString::number(m_pJoystick->axisStates()[1], 'f', 2))
                .arg(QString::number(m_pJoystick->axisStates()[2], 'f', 2))
                .arg(QString::number(m_pJoystick->axisStates()[3], 'f', 2))
                );

        CVector2 vStick = CVector2(m_pJoystick->axisStates()[0], m_pJoystick->axisStates()[1]);
        double dThrottle = 1.0 - ((m_pJoystick->axisStates()[2] + 1.0) * 0.5);

        pushData(CAirbusData(m_sName, adStick_CAPT_x, vStick.X));
        pushData(CAirbusData(m_sName, adStick_CAPT_y, vStick.Y));
        pushData(CAirbusData(m_sName, adThrottle_1_norm, dThrottle));
        pushData(CAirbusData(m_sName, adThrottle_2_norm, dThrottle));
    }
    else
    {
        if (m_bAileronLeft)
        {
            pushData(CAirbusData(m_sName, adStick_CAPT_x, -1.0));
        }
        else if (m_bAileronRight)
        {
            pushData(CAirbusData(m_sName, adStick_CAPT_x, 1.0));
        }
        else
        {
            pushData(CAirbusData(m_sName, adStick_CAPT_x, 0.0));
        }

        if (m_bNoseUp)
        {
            pushData(CAirbusData(m_sName, adStick_CAPT_y, 1.0));
        }
        else if (m_bNoseDown)
        {
            pushData(CAirbusData(m_sName, adStick_CAPT_y, -1.0));
        }
        else
        {
            pushData(CAirbusData(m_sName, adStick_CAPT_y, 0.0));
        }

        if (m_bRudderLeft)
        {
            pushData(CAirbusData(m_sName, adRudder_CAPT, -1.0));
        }
        else if (m_bRudderRight)
        {
            pushData(CAirbusData(m_sName, adRudder_CAPT, 1.0));
        }
        else
        {
            pushData(CAirbusData(m_sName, adRudder_CAPT, 0.0));
        }
    }

    sendData();
}

//-------------------------------------------------------------------------------------------------

void CAirbusController::keyPressEvent(QKeyEvent* event)
{
    CAircraftController::keyPressEvent(event);

    switch (event->key())
    {
        case Qt::Key_Space:
            generateQ3DEvent(CQ3DEvent(EventName_Jump500, CQ3DEvent::Press));
            break;
        case Qt::Key_A:
            if (m_bControlPressed)
            {
                generateQ3DEvent(CQ3DEvent(EventName_FCU_AP2, CQ3DEvent::Press));
            }
            else
            {
                generateQ3DEvent(CQ3DEvent(EventName_FCU_AP1, CQ3DEvent::Press));
            }
            break;
        case Qt::Key_T:
            generateQ3DEvent(CQ3DEvent(EventName_FCU_ATHR, CQ3DEvent::Press));
            break;
        case Qt::Key_Right:
            generateQ3DEvent(CQ3DEvent(EventName_FCU_SEL_HEADING_INC, CQ3DEvent::Press));
            break;
        case Qt::Key_Left:
            generateQ3DEvent(CQ3DEvent(EventName_FCU_SEL_HEADING_DEC, CQ3DEvent::Press));
            break;
        case Qt::Key_PageUp:
            generateQ3DEvent(CQ3DEvent(EventName_THR_THROTTLE_INC, CQ3DEvent::Press));
            break;
        case Qt::Key_PageDown:
            generateQ3DEvent(CQ3DEvent(EventName_THR_THROTTLE_DEC, CQ3DEvent::Press));
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CAirbusController::keyReleaseEvent(QKeyEvent *event)
{
    CAircraftController::keyReleaseEvent(event);
}

//-------------------------------------------------------------------------------------------------

void CAirbusController::wheelEvent(QWheelEvent *event)
{
    double dDelta = (double) event->delta() * -0.1;

    if (m_rLookTarget.component() && m_rLookTarget.component()->isCamera())
    {
        QSP<CCamera> pCamera = QSP_CAST(CCamera, m_rLookTarget.component());

        if (pCamera)
        {
            double dFOV = Math::Angles::clipDouble(pCamera->getFOV() + dDelta, 5.0, 120.0);

            pCamera->setFOV(dFOV);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CAirbusController::q3dEvent(CQ3DEvent* event)
{
    CAircraftController::q3dEvent(event);

    QSP<CAirbusFCU> pFCU = QSP_CAST(CAirbusFCU, m_rFCU.component());

    if (event->getName() == EventName_Jump500)
    {
        QSP<CComponent> pRoot = getRoot();

        if (pRoot)
        {
            QSP<CPhysicalComponent> pPhysicalRoot = QSP_CAST(CPhysicalComponent, pRoot);

            if (pPhysicalRoot)
            {
                CGeoloc gGeoloc = pPhysicalRoot->getGeoloc();
                gGeoloc.Altitude += 500.0;
                pPhysicalRoot->setGeoloc(gGeoloc);
            }
        }
    }
    else if (event->getName() == EventName_FCU_AP1)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pFCU)
            {
                pFCU->toggle_AutoPilot1_Engaged();
            }
        }
    }
    else if (event->getName() == EventName_FCU_AP2)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pFCU)
            {
                pFCU->toggle_AutoPilot2_Engaged();
            }
        }
    }
    else if (event->getName() == EventName_FCU_ATHR)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pFCU)
            {
                pFCU->toggle_AutoThrust_Engaged();
            }
        }
    }
    else if (event->getName() == EventName_FCU_SEL_HEADING_INC)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pFCU)
            {
                pFCU->increment_SelectedHeading(false);
            }
        }
    }
    else if (event->getName() == EventName_FCU_SEL_HEADING_DEC)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            if (pFCU)
            {
                pFCU->decrement_SelectedHeading(false);
            }
        }
    }
    else if (event->getName() == EventName_THR_THROTTLE_INC)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            CAirbusData* pThrottle_1_norm = getData(adThrottle_1_norm);
            CAirbusData* pThrottle_2_norm = getData(adThrottle_2_norm);

            double dThrottle_1_norm = 0.0;
            double dThrottle_2_norm = 0.0;

            if (pThrottle_1_norm != NULL)
            {
                dThrottle_1_norm = pThrottle_1_norm->getData().toDouble();
            }

            if (pThrottle_2_norm != NULL)
            {
                dThrottle_2_norm = pThrottle_2_norm->getData().toDouble();
            }

            dThrottle_1_norm = Math::Angles::clipDouble(dThrottle_1_norm + 0.1, 0.0, 1.0);
            dThrottle_2_norm = Math::Angles::clipDouble(dThrottle_2_norm + 0.1, 0.0, 1.0);

            pushData(CAirbusData(m_sName, adThrottle_1_norm, dThrottle_1_norm));
            pushData(CAirbusData(m_sName, adThrottle_2_norm, dThrottle_2_norm));
        }
    }
    else if (event->getName() == EventName_THR_THROTTLE_DEC)
    {
        if (event->getAction() == CQ3DEvent::Press)
        {
            CAirbusData* pThrottle_1_norm = getData(adThrottle_1_norm);
            CAirbusData* pThrottle_2_norm = getData(adThrottle_2_norm);

            double dThrottle_1_norm = 0.0;
            double dThrottle_2_norm = 0.0;

            if (pThrottle_1_norm != NULL)
            {
                dThrottle_1_norm = pThrottle_1_norm->getData().toDouble();
            }

            if (pThrottle_2_norm != NULL)
            {
                dThrottle_2_norm = pThrottle_2_norm->getData().toDouble();
            }

            dThrottle_1_norm = Math::Angles::clipDouble(dThrottle_1_norm - 0.1, 0.0, 1.0);
            dThrottle_2_norm = Math::Angles::clipDouble(dThrottle_2_norm - 0.1, 0.0, 1.0);

            pushData(CAirbusData(m_sName, adThrottle_1_norm, dThrottle_1_norm));
            pushData(CAirbusData(m_sName, adThrottle_2_norm, dThrottle_2_norm));
        }
    }
}
