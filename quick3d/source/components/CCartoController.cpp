
#include "CVector3.h"
#include "CAxis.h"

#include "C3DScene.h"
#include "CCartoController.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CCartoController::instantiator(C3DScene* pScene)
{
    return new CCartoController(pScene);
}

//-------------------------------------------------------------------------------------------------

CCartoController::CCartoController(C3DScene* pScene)
    : CController(pScene)
    , m_bUseMouse(false)
    , m_bPanTilt(false)
{
}

//-------------------------------------------------------------------------------------------------

CCartoController::~CCartoController()
{
}

//-------------------------------------------------------------------------------------------------

void CCartoController::update(double dDeltaTime)
{
    CController::update(dDeltaTime);
}

//-------------------------------------------------------------------------------------------------

void CCartoController::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Escape:
            m_bUseMouse = false;
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CCartoController::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    }
}

//-------------------------------------------------------------------------------------------------

void CCartoController::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bUseMouse = true;
    }
    else
    {
        m_bPanTilt = true;
    }

    m_pPreviousMousePos = event->globalPos();
}

//-------------------------------------------------------------------------------------------------

void CCartoController::mouseReleaseEvent(QMouseEvent* event)
{
    m_bUseMouse = false;
    m_bPanTilt = false;
}

//-------------------------------------------------------------------------------------------------

void CCartoController::mouseMoveEvent(QMouseEvent* event)
{
    QPoint point = event->globalPos();

    double dDeltaY = (double) (point.x() - m_pPreviousMousePos.x());
    double dDeltaX = (double) (point.y() - m_pPreviousMousePos.y());

    if (m_bUseMouse)
    {
        if (m_rPositionTarget.component())
        {
            double dHeading = m_rRotationTarget.component()->rotation().Y;
            double dMoveFactor = m_rPositionTarget.component()->geoloc().Altitude * 0.00000005;

            CVector3 vMovement(-dDeltaY * dMoveFactor, 0.0, dDeltaX * dMoveFactor);

            vMovement = CMatrix4().makeRotation(CVector3(0.0, dHeading, 0.0)) * vMovement;

            CGeoloc gPosition = m_rPositionTarget.component()->geoloc();

            gPosition.Latitude += vMovement.Z;
            gPosition.Longitude += vMovement.X;

            m_rPositionTarget.component()->setGeoloc(gPosition);
        }
    }
    else if (m_bPanTilt)
    {
        if (m_rRotationTarget.component())
        {
            CVector3 vRotationX = CVector3(dDeltaX * 0.005, 0.0, 0.0);
            CVector3 vRotationY = CVector3(0.0, dDeltaY * 0.005, 0.0);

            m_rRotationTarget.component()->setRotation(m_rRotationTarget.component()->rotation() + vRotationX);
            m_rRotationTarget.component()->setRotation(m_rRotationTarget.component()->rotation() + vRotationY);
        }
    }

    m_pPreviousMousePos = point;
}

//-------------------------------------------------------------------------------------------------

void CCartoController::wheelEvent(QWheelEvent *event)
{
    double dDeltaY = (double) event->delta() * -0.25;

    if (m_rPositionTarget.component())
    {
        double dMoveFactor = m_rPositionTarget.component()->geoloc().Altitude * 0.005;
        if (dMoveFactor < 1.0) dMoveFactor = 1.0;
        CGeoloc gPosition = m_rPositionTarget.component()->geoloc();
        gPosition.Altitude += dDeltaY * dMoveFactor;
        if (gPosition.Altitude < 20.0) gPosition.Altitude = 20.0;
        m_rPositionTarget.component()->setGeoloc(gPosition);
    }
}
