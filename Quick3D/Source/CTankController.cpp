
// Fondations
#include "CVector3.h"
#include "CAxis.h"

// Application
#include "C3DScene.h"
#include "CTankController.h"

using namespace Math;

#define TURN_SPEED	15.0

//-------------------------------------------------------------------------------------------------

CComponent* CTankController::instanciator(C3DScene* pScene)
{
    return new CTankController(pScene);
}

//-------------------------------------------------------------------------------------------------

CTankController::CTankController(C3DScene* pScene)
    : CForceController(pScene)
    , m_bTurretUp(false)
    , m_bTurretDown(false)
    , m_bTurretRight(false)
    , m_bTurretLeft(false)
{
}

//-------------------------------------------------------------------------------------------------

CTankController::~CTankController()
{
}

//-------------------------------------------------------------------------------------------------

void CTankController::loadParameters(const QString& sBaseFile, CXMLNode xNode)
{
    CForceController::loadParameters(sBaseFile, xNode);

    m_rTurretYawTarget.setName(xNode.attributes()["TurretYawTarget"]);
    m_rTurretPitchTarget.setName(xNode.attributes()["TurretPitchTarget"]);
}

//-------------------------------------------------------------------------------------------------

void CTankController::solveLinks(C3DScene* pScene)
{
    CForceController::solveLinks(pScene);

    m_rTurretYawTarget.solve(pScene, this);
    m_rTurretPitchTarget.solve(pScene, this);
}

//-------------------------------------------------------------------------------------------------

void CTankController::update(double dDeltaTime)
{
    CForceController::update(dDeltaTime);

    CComponent* m_pTurretYawTarget = m_rTurretYawTarget.component();
    CComponent* m_pTurretPitchTarget = m_rTurretPitchTarget.component();

    if (m_pTurretYawTarget)
    {
        double dRotationSpeed = Math::Angles::toRad(TURN_SPEED);

        if (m_bTurretRight)
        {
            CVector3 vRotation = CVector3(0.0, dRotationSpeed * dDeltaTime, 0.0);
            m_pTurretYawTarget->setOriginRotation(m_pTurretYawTarget->getOriginRotation() + vRotation);
        }
        else
            if (m_bTurretLeft)
            {
                CVector3 vRotation = CVector3(0.0, -dRotationSpeed * dDeltaTime, 0.0);
                m_pTurretYawTarget->setOriginRotation(m_pTurretYawTarget->getOriginRotation() + vRotation);
            }
    }

    if (m_pTurretPitchTarget)
    {
        double dRotationSpeed = Math::Angles::toRad(TURN_SPEED);

        if (m_bTurretUp)
        {
            CVector3 vRotation = CVector3(dRotationSpeed * dDeltaTime, 0.0, 0.0);
            m_pTurretPitchTarget->setOriginRotation(m_pTurretPitchTarget->getOriginRotation() + vRotation);
        }
        else
            if (m_bTurretDown)
            {
                CVector3 vRotation = CVector3(-dRotationSpeed * dDeltaTime, 0.0, 0.0);
                m_pTurretPitchTarget->setOriginRotation(m_pTurretPitchTarget->getOriginRotation() + vRotation);
            }
    }
}

//-------------------------------------------------------------------------------------------------

void CTankController::keyPressEvent(QKeyEvent* event)
{
    CForceController::keyPressEvent(event);

    switch (event->key())
    {
        case Qt::Key_Right:
            m_bTurretRight = true;
            break;
        case Qt::Key_Left:
            m_bTurretLeft = true;
            break;
        case Qt::Key_Up:
            m_bTurretUp = true;
            break;
        case Qt::Key_Down:
            m_bTurretDown = true;
            break;
    }
}

//-------------------------------------------------------------------------------------------------

void CTankController::keyReleaseEvent(QKeyEvent *event)
{
    CForceController::keyReleaseEvent(event);

    switch (event->key())
    {
        case Qt::Key_Right: m_bTurretRight = false; break;
        case Qt::Key_Left: m_bTurretLeft = false; break;
        case Qt::Key_Up: m_bTurretUp = false; break;
        case Qt::Key_Down: m_bTurretDown = false; break;
    }
}
