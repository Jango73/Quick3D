
// COTS
#include <SFML/Window/Joystick.hpp>

// Application
#include "CVector2.h"
#include "CVector3.h"
#include "CAxis.h"
#include "CController.h"
#include "CTrajectorable.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CController::CController(C3DScene* pScene)
    : CComponent(pScene)
    , m_pJoystick(nullptr)
    , m_bUseMouse(false)
    , m_bControlPressed(false)
    , m_bAltPressed(false)
    , m_dMoveSpeed(0.0)
    , m_dForceFactor(1.0)
{
    sf::Joystick::Update();

    for (int iJoystickIndex = 0; iJoystickIndex < sf::Joystick::Count; iJoystickIndex++)
    {
        if (sf::Joystick::IsConnected(iJoystickIndex))
        {
            m_pJoystick = new CJoystick(iJoystickIndex);
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------

CController::~CController()
{
    if (m_pJoystick != nullptr)
    {
        delete m_pJoystick;
    }
}

//-------------------------------------------------------------------------------------------------

void CController::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CComponent::loadParameters(sBaseFile, xComponent);

    m_rPositionTarget.setName(xComponent.attributes()[ParamName_PositionTarget]);
    m_rRotationTarget.setName(xComponent.attributes()[ParamName_RotationTarget]);
    m_rLookTarget.setName(xComponent.attributes()[ParamName_LookTarget]);

    if (xComponent.attributes()[ParamName_ForceFactor].isEmpty() == false)
    {
        m_dForceFactor = xComponent.attributes()[ParamName_ForceFactor].toDouble();
    }
}

//-------------------------------------------------------------------------------------------------

void CController::solveLinks(C3DScene* pScene)
{
    CComponent::solveLinks(pScene);

    m_rPositionTarget.solve(pScene, QSP<CComponent>(this));
    m_rRotationTarget.solve(pScene, QSP<CComponent>(this));
    m_rLookTarget.solve(pScene, QSP<CComponent>(this));

    if (!m_rPositionTarget.component() && m_pParent)
    {
        m_rPositionTarget.setComponent(m_pParent);
    }

    if (!m_rRotationTarget.component() && m_pParent)
    {
        m_rRotationTarget.setComponent(m_pParent);
    }
}

//-------------------------------------------------------------------------------------------------

void CController::clearLinks(C3DScene* pScene)
{
    CComponent::clearLinks(pScene);

    m_rPositionTarget.clear();
    m_rRotationTarget.clear();
    m_rLookTarget.clear();
}

//-------------------------------------------------------------------------------------------------

void CController::update(double dDeltaTime)
{
    CComponent::update(dDeltaTime);

    if (m_pJoystick != nullptr)
    {
        sf::Joystick::Update();
        m_pJoystick->update(dDeltaTime);
    }

    m_pScene->addSegment(m_rLastRay.vOrigin, m_rLastRay.vOrigin + m_rLastRay.vNormal);
}

//-------------------------------------------------------------------------------------------------

void CController::setPositionTarget(QSP<CComponent> pComponent)
{
    if (m_rPositionTarget.component())
    {
        if (m_rPositionTarget.component()->isTrajectorable())
        {
            QSP<CTrajectorable> pTraj = QSP_CAST(CTrajectorable, m_rPositionTarget.component());

            if (pTraj != nullptr)
            {
                pTraj->setTrajectoryEnabled(true);
            }
        }
    }

    m_rPositionTarget.setComponent(pComponent);

    if (m_rPositionTarget.component()->isTrajectorable())
    {
        QSP<CTrajectorable> pTraj = QSP_CAST(CTrajectorable, m_rPositionTarget.component());

        if (pTraj != nullptr)
        {
            pTraj->setTrajectoryEnabled(false);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CController::setRotationTarget(QSP<CComponent> pComponent)
{
    m_rRotationTarget.setComponent(pComponent);
}

//-------------------------------------------------------------------------------------------------

void CController::generateQ3DEvent(CQ3DEvent anEvent)
{
    q3dEvent(&anEvent);
}

//-------------------------------------------------------------------------------------------------

void CController::generateQ3DMouseEvent(CQ3DEvent::EEventAction eAction, QMouseEvent* event, CComponent* pComponent)
{
    QStringList sNameList = pComponent->name().split(":");

    if (sNameList.count() == 3)
    {
        QString sEvent = sNameList[0];
        QString sType = sNameList[1];
        QString sName = sNameList[2];

        if (sEvent == Q3D_Event)
        {
            bool bProcess = false;

            CQ3DEvent anEvent(sName, eAction);

            switch (event->button())
            {
                case Qt::LeftButton:
                    if (sType == Q3D_LeftMouseButton)
                        m_eCurrentLeftMouseEvent.setName(sName);
                    bProcess = true;
                    break;

                case Qt::RightButton:
                    if (sType == Q3D_RightMouseButton)
                        m_eCurrentRightMouseEvent.setName(sName);
                    bProcess = true;
                    break;

                case Qt::MidButton:
                    if (sType == Q3D_MiddleMouseButton)
                        m_eCurrentRightMouseEvent.setName(sName);
                    bProcess = true;
                    break;
                default:
                    break;
            }

            if (bProcess == true)
            {
                q3dEvent(&anEvent);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CController::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Control:
            m_bControlPressed = true;
            break;
        case Qt::Key_Alt:
            m_bAltPressed = true;
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CController::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Control:
            m_bControlPressed = false;
            break;
        case Qt::Key_Alt:
            m_bAltPressed = false;
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CController::mousePressEvent(QMouseEvent* event)
{
    m_bUseMouse = true;
    m_pPreviousMousePos = event->globalPos();

    if (m_pParent != nullptr) // && event->button() == Qt::RightButton)
    {
        C3DScene* pScene = m_pParent->scene();

        if (pScene != nullptr)
        {
            QPointF point = event->localPos();

            foreach (CViewport* pViewport, pScene->viewports().values())
            {
                if (pViewport != nullptr)
                {
                    if (
                            point.x() >= pViewport->getPosition().X &&
                            point.y() >= pViewport->getPosition().Y &&
                            point.x() < pViewport->getPosition().X + pViewport->getSize().X &&
                            point.y() < pViewport->getPosition().Y + pViewport->getSize().Y
                            )
                    {
                        QSP<CCamera> pCamera = pViewport->getCamera();

                        if (pCamera != nullptr)
                        {
                            CVector2 normalizedPoint(
                                        (point.x() - pViewport->getPosition().X) / pViewport->getSize().X,
                                        (point.y() - pViewport->getPosition().Y) / pViewport->getSize().Y
                                        );

                            CVector2 vAngles2 = CVector2::pointToAngles(pViewport->getSize(), pCamera->getFOV(), normalizedPoint);
                            CVector3 vAngles3(vAngles2.Y, vAngles2.X, 0.0);

                            m_rLastRay.vOrigin = pCamera->worldTransform() * CVector3();
                            m_rLastRay.vNormal = CMatrix4::makeRotation(vAngles3) * CVector3(0.0, 0.0, 1.0);
                            m_rLastRay.vNormal = (pCamera->worldTransform() * m_rLastRay.vNormal) - m_rLastRay.vOrigin;

                            RayTracingResult aResult = pScene->intersectComponentHierarchy(m_pParent, m_rLastRay);

                            if (aResult.m_dDistance < Q3D_INFINITY)
                            {
                                if (aResult.m_pObject != nullptr)
                                {
                                    CComponent* pComponent = (CComponent*)aResult.m_pObject;

                                    generateQ3DMouseEvent(CQ3DEvent::Press, event, pComponent);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CController::mouseReleaseEvent(QMouseEvent* event)
{
    m_bUseMouse = false;
}

//-------------------------------------------------------------------------------------------------

void CController::mouseMoveEvent(QMouseEvent* event)
{
}

//-------------------------------------------------------------------------------------------------

void CController::wheelEvent(QWheelEvent *event)
{
}

//-------------------------------------------------------------------------------------------------

void CController::joystickEvent(CJoystick* pJoystick)
{
}

//-------------------------------------------------------------------------------------------------

void CController::q3dEvent(CQ3DEvent* event)
{
}
