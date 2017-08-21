
// Application
#include "CConsoleBoard.h"
#include "CVector3.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CQuadDroneController.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CQuadDroneController::instanciator(C3DScene* pScene)
{
    return new CQuadDroneController(pScene);
}

//-------------------------------------------------------------------------------------------------

CQuadDroneController::CQuadDroneController(C3DScene* pScene)
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

CQuadDroneController::~CQuadDroneController()
{
}

//-------------------------------------------------------------------------------------------------

void CQuadDroneController::loadParameters(const QString& sBaseFile, CXMLNode xNode)
{
    CStandardController::loadParameters(sBaseFile, xNode);

    m_rEngine1Target.setName(xNode.attributes()[ParamName_Engine1Target]);
    m_rEngine2Target.setName(xNode.attributes()[ParamName_Engine2Target]);
    m_rEngine3Target.setName(xNode.attributes()[ParamName_Engine3Target]);
    m_rEngine4Target.setName(xNode.attributes()[ParamName_Engine4Target]);
}

//-------------------------------------------------------------------------------------------------

void CQuadDroneController::solveLinks(C3DScene* pScene)
{
    CStandardController::solveLinks(pScene);

    m_rEngine1Target.solve(pScene, QSP<CComponent>(this));
    m_rEngine2Target.solve(pScene, QSP<CComponent>(this));
    m_rEngine3Target.solve(pScene, QSP<CComponent>(this));
    m_rEngine4Target.solve(pScene, QSP<CComponent>(this));
}

//-------------------------------------------------------------------------------------------------

void CQuadDroneController::clearLinks(C3DScene* pScene)
{
    CStandardController::clearLinks(pScene);

    m_rEngine1Target.clear();
    m_rEngine2Target.clear();
    m_rEngine3Target.clear();
    m_rEngine4Target.clear();
}

//-------------------------------------------------------------------------------------------------

void CQuadDroneController::update(double dDeltaTime)
{
    CController::update(dDeltaTime);

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
    }
}

//-------------------------------------------------------------------------------------------------

void CQuadDroneController::keyPressEvent(QKeyEvent* event)
{
    CStandardController::keyPressEvent(event);

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
    }
}

//-------------------------------------------------------------------------------------------------

void CQuadDroneController::keyReleaseEvent(QKeyEvent *event)
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

void CQuadDroneController::q3dEvent(CQ3DEvent* event)
{
    CStandardController::q3dEvent(event);

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
}
