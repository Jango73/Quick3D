
// qt-plus
#include "CLogger.h"

// Application
#include "Angles.h"
#include "CAxis.h"
#include "CTrajectory.h"
#include "CTrajectorable.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CTrajectory::CTrajectory(bool bAutoOrientation)
    : m_iCurrentPoint(0)
    , m_bAutoOrientation(bAutoOrientation)
{
}

//-------------------------------------------------------------------------------------------------

CTrajectory::~CTrajectory()
{
}

//-------------------------------------------------------------------------------------------------

void CTrajectory::setPosition(CGeoloc Position)
{
    m_gPosition = Position;
}

//-------------------------------------------------------------------------------------------------

void CTrajectory::setRotation(CVector3 Rotation)
{
    m_vRotation = Rotation;
}

//-------------------------------------------------------------------------------------------------

CGeoloc CTrajectory::position()
{
    return m_gPosition;
}

//-------------------------------------------------------------------------------------------------

CVector3 CTrajectory::rotation()
{
    return m_vRotation;
}

//-------------------------------------------------------------------------------------------------

void CTrajectory::reset()
{
    m_iCurrentPoint = 0;
}

//-------------------------------------------------------------------------------------------------

void CTrajectory::addPoint(CGeoloc vPoint)
{
    m_vPoints.append(vPoint);
}

//-------------------------------------------------------------------------------------------------

void CTrajectory::processObject(CTrajectorable* pObject, double dDeltaTime)
{
    if (m_vPoints.count() > 0)
    {
        CGeoloc gTarget = m_vPoints[m_iCurrentPoint];
        CGeoloc gPosition = pObject->geoloc();

        CGeoloc gTargetGround = CGeoloc(gTarget.Latitude, gTarget.Longitude, 0.0);
        CGeoloc gPositionGround = CGeoloc(gPosition.Latitude, gPosition.Longitude, 0.0);

        CVector3 vDirection = gTargetGround.toVector3(gPositionGround);

        if ((vDirection).magnitude() < 10.0)
        {
            m_iCurrentPoint++;
            if (m_iCurrentPoint >= m_vPoints.count()) m_iCurrentPoint = 0;
            return;

            LOG_METHOD_DEBUG(QString("%1 reached waypoint, next = %2")
                      .arg(pObject->name())
                      .arg(m_iCurrentPoint));
        }

        double dSpeedMS = pObject->getSpeedMS();
        double dTurnSpeedDS = pObject->getTurnSpeedDS();

        CVector3 vRotation = pObject->rotation();
        double dCurrentAngleY = vRotation.Y;
        double dTargetAngleY = vDirection.eulerYAngle();
        double dDiffAngleY = Math::Angles::angleDifferenceRadian(dTargetAngleY, dCurrentAngleY) * 2.0;

        if (dDiffAngleY >  1.0) dDiffAngleY =  1.0;
        if (dDiffAngleY < -1.0) dDiffAngleY = -1.0;

        vRotation.Y += (dDiffAngleY * Angles::toRad(dTurnSpeedDS)) * dDeltaTime;

        CAxis aLocalAxis(pObject->ECEFRotation());
        pObject->setPosition(pObject->position() + aLocalAxis.Front * (dSpeedMS * dDeltaTime));
        pObject->setRotation(vRotation);
    }
}
