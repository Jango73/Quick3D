
// Application
#include "CVector3.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CStandardController.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

#define TURN_SPEED			40.0

//-------------------------------------------------------------------------------------------------

CComponent* CStandardController::instantiator(C3DScene* pScene)
{
    return new CStandardController(pScene);
}

//-------------------------------------------------------------------------------------------------

CStandardController::CStandardController(C3DScene* pScene)
    : CController(pScene)
    , m_bLookUp(false)
    , m_bLookDown(false)
    , m_bGoForward(false)
    , m_bGoBackward(false)
    , m_bGoUp(false)
    , m_bGoDown(false)
    , m_bTurnRight(false)
    , m_bTurnLeft(false)
    , m_bStrafeRight(false)
    , m_bStrafeLeft(false)
    , m_bAltitudeFastUp(false)
    , m_bAltitudeFastDown(false)
{
}

//-------------------------------------------------------------------------------------------------

CStandardController::~CStandardController()
{
}

//-------------------------------------------------------------------------------------------------

void CStandardController::update(double dDeltaTime)
{
    CController::update(dDeltaTime);

    double dMoveSpeed = 1.5 + (m_dMoveSpeed * 300.0);

    QSP<CComponent> pPositionTarget = m_rPositionTarget.component();
    QSP<CComponent> pRotationTarget = m_rRotationTarget.component();
    QSP<CComponent> pLookTarget = m_rLookTarget.component();

    if (pPositionTarget != nullptr)
    {
        double dMoveFactor = pPositionTarget->geoloc().Altitude / 500.0;
        if (dMoveFactor < 1.0) dMoveFactor = 1.0;

        dMoveSpeed = dMoveSpeed * dMoveFactor;

        if (m_bGoForward)
        {
            CAxis aLocalAxis(pPositionTarget->ECEFRotation());
            pPositionTarget->setPosition(pPositionTarget->position() + aLocalAxis.Front * (dMoveSpeed * dDeltaTime));
        }
        else if (m_bGoBackward)
        {
            CAxis aLocalAxis(pPositionTarget->ECEFRotation());
            pPositionTarget->setPosition(pPositionTarget->position() + aLocalAxis.Front * (-dMoveSpeed * dDeltaTime));
        }

        if (m_bGoUp)
        {
            CAxis aLocalAxis(pPositionTarget->ECEFRotation());
            pPositionTarget->setPosition(pPositionTarget->position() + aLocalAxis.Up * (dMoveSpeed * dDeltaTime));
        }
        else if (m_bGoDown)
        {
            CAxis aLocalAxis(pPositionTarget->ECEFRotation());
            pPositionTarget->setPosition(pPositionTarget->position() + aLocalAxis.Up * (-dMoveSpeed * dDeltaTime));
        }

        if (m_bStrafeRight)
        {
            CAxis aLocalAxis(pPositionTarget->ECEFRotation());
            pPositionTarget->setPosition(pPositionTarget->position() + aLocalAxis.Right * (dMoveSpeed * dDeltaTime));

            /*
            // Following is used when rotation is absolute
            Axis aOriginAxis(m_pRotationTarget->rotation());
            Axis aNorthAxis = m_pRotationTarget->geoloc().getNOLLAxis();
            aOriginAxis = aOriginAxis.transferFrom(aNorthAxis);
            m_pPositionTarget->setPosition(m_pPositionTarget->getPosition() + aOriginAxis.Right * (dMoveSpeed * dDeltaTime));
            */
        }
        else if (m_bStrafeLeft)
        {
            CAxis aLocalAxis(pPositionTarget->ECEFRotation());
            pPositionTarget->setPosition(pPositionTarget->position() + aLocalAxis.Right * (-dMoveSpeed * dDeltaTime));
        }

#define MAX_ALT 50000000.0

        if (m_bAltitudeFastUp)
        {
            CGeoloc gPosition = pPositionTarget->geoloc();
            double dFactor = (1.0 - (gPosition.Altitude / MAX_ALT)) * 500000.0;
            if (dFactor < 100.0) dFactor = 100.0;
            gPosition.Altitude += dFactor * dDeltaTime;
            if (gPosition.Altitude > MAX_ALT) gPosition.Altitude = MAX_ALT;
            pPositionTarget->setGeoloc(gPosition);
        }
        else if (m_bAltitudeFastDown)
        {
            CGeoloc gPosition = pPositionTarget->geoloc();
            double dFactor = (1.0 - (gPosition.Altitude / MAX_ALT)) * 500000.0;
            if (dFactor < 100.0) dFactor = 100.0;
            gPosition.Altitude -= dFactor * dDeltaTime;
            if (gPosition.Altitude > MAX_ALT) gPosition.Altitude = MAX_ALT;
            pPositionTarget->setGeoloc(gPosition);
        }
    }

    if (pRotationTarget != nullptr)
    {
        if (m_bTurnRight)
        {
            CVector3 vRotation = CVector3(0.0, Math::Angles::toRad(TURN_SPEED) * dDeltaTime, 0.0);

            /*
            // Following is used when rotation is absolute
            if (m_pRotationTarget->isRootObject())
            {
                Axis aOriginAxis = Axis(m_pRotationTarget->rotation());
                Axis aNorthAxis = m_pRotationTarget->geoloc().getNOLLAxis();
                aOriginAxis = aOriginAxis.transferFrom(aNorthAxis);
                aOriginAxis = aOriginAxis.rotate(vRotation);
                aOriginAxis = aOriginAxis.transferTo(aNorthAxis);
                m_pRotationTarget->setRotation(aOriginAxis.euleurAngles());
            }
            else
            {
                m_pRotationTarget->setRotation(m_pRotationTarget->rotation() + vRotation);
            }
            */

            pRotationTarget->setRotation(pRotationTarget->rotation() + vRotation);
        }
        else if (m_bTurnLeft)
        {
            CVector3 vRotation = CVector3(0.0, Math::Angles::toRad(-TURN_SPEED) * dDeltaTime, 0.0);

            /*
            // Following is used when rotation is absolute
            if (m_pRotationTarget->isRootObject())
            {
                Axis aOriginAxis = Axis(m_pRotationTarget->rotation());
                Axis aNorthAxis = m_pRotationTarget->geoloc().getNOLLAxis();
                aOriginAxis = aOriginAxis.transferFrom(aNorthAxis);
                aOriginAxis = aOriginAxis.rotate(vRotation);
                aOriginAxis = aOriginAxis.transferTo(aNorthAxis);
                m_pRotationTarget->setRotation(aOriginAxis.euleurAngles());
            }
            else
            {
                m_pRotationTarget->setRotation(m_pRotationTarget->rotation() + vRotation);
            }
            */

            pRotationTarget->setRotation(pRotationTarget->rotation() + vRotation);
        }
    }

    if (pLookTarget != nullptr)
    {
        if (m_bLookUp)
        {
            CVector3 vRotation = CVector3(Math::Angles::toRad(TURN_SPEED) * dDeltaTime, 0.0, 0.0);

            // Following is used when rotation is absolute
            // m_pRotationTarget->setRotation(Axis(vRotation).transferTo(Axis(m_pRotationTarget->rotation())).eulerAngles());

            pRotationTarget->setRotation(pRotationTarget->rotation() + vRotation);
        }
        else if (m_bLookDown)
        {
            CVector3 vRotation = CVector3(Math::Angles::toRad(-TURN_SPEED) * dDeltaTime, 0.0, 0.0);

            // Following is used when rotation is absolute
            // m_pRotationTarget->setRotation(Axis(vRotation).transferTo(Axis(m_pRotationTarget->rotation())).eulerAngles());

            pRotationTarget->setRotation(pRotationTarget->rotation() + vRotation);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CStandardController::keyPressEvent(QKeyEvent* event)
{
    CController::keyPressEvent(event);

    switch (event->key())
    {
        case Qt::Key_Escape:
            m_bUseMouse = false;
            break;
        case Qt::Key_A:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookUp, CQ3DEvent::Press));
            break;
        case Qt::Key_E:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookDown, CQ3DEvent::Press));
            break;
        case Qt::Key_Z:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Forward, CQ3DEvent::Press));
            break;
        case Qt::Key_S:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Backward, CQ3DEvent::Press));
            break;
        case Qt::Key_C:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_TurnRight, CQ3DEvent::Press));
            break;
        case Qt::Key_W:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_TurnLeft, CQ3DEvent::Press));
            break;
        case Qt::Key_D:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_StrafeRight, CQ3DEvent::Press));
            break;
        case Qt::Key_Q:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_StrafeLeft, CQ3DEvent::Press));
            break;
        case Qt::Key_Space:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Up, CQ3DEvent::Press));
            break;
        case Qt::Key_PageDown:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Down, CQ3DEvent::Press));
            break;
        case Qt::Key_O:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_UpFast, CQ3DEvent::Press));
            break;
        case Qt::Key_L:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_DownFast, CQ3DEvent::Press));
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CStandardController::keyReleaseEvent(QKeyEvent *event)
{
    CController::keyReleaseEvent(event);

    switch (event->key())
    {
        case Qt::Key_A:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookUp, CQ3DEvent::Release));
            break;
        case Qt::Key_E:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_LookDown, CQ3DEvent::Release));
            break;
        case Qt::Key_Z:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Forward, CQ3DEvent::Release));
            break;
        case Qt::Key_S:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Backward, CQ3DEvent::Release));
            break;
        case Qt::Key_C:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_TurnRight, CQ3DEvent::Release));
            break;
        case Qt::Key_W:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_TurnLeft, CQ3DEvent::Release));
            break;
        case Qt::Key_D:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_StrafeRight, CQ3DEvent::Release));
            break;
        case Qt::Key_Q:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_StrafeLeft, CQ3DEvent::Release));
            break;
        case Qt::Key_Space:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Up, CQ3DEvent::Release));
            break;
        case Qt::Key_PageDown:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_Down, CQ3DEvent::Release));
            break;
        case Qt::Key_O:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_UpFast, CQ3DEvent::Release));
            break;
        case Qt::Key_L:
            generateQ3DEvent(CQ3DEvent(Q3DEvent_DownFast, CQ3DEvent::Release));
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CStandardController::mouseMoveEvent(QMouseEvent* event)
{
    QSP<CComponent> pPositionTarget = m_rPositionTarget.component();
    QSP<CComponent> pRotationTarget = m_rRotationTarget.component();
    QSP<CComponent> pLookTarget = m_rLookTarget.component();

    if (m_bUseMouse)
    {
        QPoint point = event->globalPos();

        double dDeltaY = (double) (point.x() - m_pPreviousMousePos.x());
        double dDeltaX = (double) (point.y() - m_pPreviousMousePos.y());

        if (m_bControlPressed)
        {
            if (pPositionTarget != nullptr)
            {
                double dMoveFactor = pPositionTarget->geoloc().Altitude * 0.00000005;
                CGeoloc gPosition = pPositionTarget->geoloc();
                gPosition.Latitude += dDeltaX * dMoveFactor;
                gPosition.Longitude -= dDeltaY * dMoveFactor;
                pPositionTarget->setGeoloc(gPosition);
            }
        }
        else if (m_bAltPressed)
        {
            if (pRotationTarget != nullptr)
            {
                pRotationTarget->setRotation(CVector3(Math::Pi * 0.5, 0.0, 0.0));
            }

            if (pPositionTarget != nullptr)
            {
                double dMoveFactor = pPositionTarget->geoloc().Altitude * 0.005;
                if (dMoveFactor < 1.0) dMoveFactor = 1.0;
                CGeoloc gPosition = pPositionTarget->geoloc();
                gPosition.Altitude += dDeltaX * dMoveFactor;
                pPositionTarget->setGeoloc(gPosition);
            }
        }
        else
        {
            if (pLookTarget != nullptr)
            {
                CVector3 vRotationX = CVector3(dDeltaX * 0.005, 0.0, 0.0);
                CVector3 vRotationY = CVector3(0.0, dDeltaY * 0.005, 0.0);

                pLookTarget->setRotation(pLookTarget->rotation() + vRotationX);
                pLookTarget->setRotation(pLookTarget->rotation() + vRotationY);

                /*
                // Following is used when rotation is absolute
                m_pRotationTarget->setRotation(Axis(vRotationX).transferTo(Axis(m_pRotationTarget->rotation())).eulerAngles());

                if (m_pRotationTarget->isRootObject())
                {
                    Axis aOriginAxis(m_pRotationTarget->rotation());
                    Axis aNorthAxis = m_pRotationTarget->geoloc().getNOLLAxis();
                    aOriginAxis = aOriginAxis.transferFrom(aNorthAxis);
                    aOriginAxis = aOriginAxis.rotate(vRotationY);
                    aOriginAxis = aOriginAxis.transferTo(aNorthAxis);
                    m_pRotationTarget->setRotation(aOriginAxis.eulerAngles());
                }
                else
                {
                    m_pRotationTarget->setRotation(m_pRotationTarget->rotation() + vRotationY);
                }
                */
            }
        }

        m_pPreviousMousePos = point;
    }
}

//-------------------------------------------------------------------------------------------------

void CStandardController::q3dEvent(CQ3DEvent* event)
{
    CController::q3dEvent(event);

    if (event->getName() == Q3DEvent_Forward)
    {
        m_bGoForward = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_Backward)
    {
        m_bGoBackward = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_TurnRight)
    {
        m_bTurnRight = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_TurnLeft)
    {
        m_bTurnLeft = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_LookUp)
    {
        m_bLookUp = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_LookDown)
    {
        m_bLookDown = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_StrafeRight)
    {
        m_bStrafeRight = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_StrafeLeft)
    {
        m_bStrafeLeft = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_Up)
    {
        m_bGoUp = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_Down)
    {
        m_bGoDown = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_UpFast)
    {
        m_bAltitudeFastUp = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_DownFast)
    {
        m_bAltitudeFastDown = (event->getAction() == CQ3DEvent::Press);
    }
    else if (event->getName() == Q3DEvent_ToggleEdit && event->getAction() == CQ3DEvent::Press)
    {
        m_pScene->setEditMode(!m_pScene->editMode());
    }
}
