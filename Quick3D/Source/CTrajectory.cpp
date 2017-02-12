
// Fondations
#include "CLogManager.h"
#include "Angles.h"
#include "CAxis.h"

// Application
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

void CTrajectory::setOriginPosition(CGeoloc Position)
{
    m_gOriginPosition = Position;
}

//-------------------------------------------------------------------------------------------------

void CTrajectory::setOriginRotation(CVector3 Rotation)
{
    m_vOriginRotation = Rotation;
}

//-------------------------------------------------------------------------------------------------

CGeoloc CTrajectory::getOriginPosition()
{
    return m_gOriginPosition;
}

//-------------------------------------------------------------------------------------------------

CVector3 CTrajectory::getOriginRotation()
{
    return m_vOriginRotation;
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
        CGeoloc gPosition = pObject->getGeoloc();

        CGeoloc gTargetGround = CGeoloc(gTarget.Latitude, gTarget.Longitude, 0.0);
        CGeoloc gPositionGround = CGeoloc(gPosition.Latitude, gPosition.Longitude, 0.0);

        CVector3 vDirection = gTargetGround.toVector3(gPositionGround);

        if ((vDirection).getMagnitude() < 10.0)
        {
            m_iCurrentPoint++;
            if (m_iCurrentPoint >= m_vPoints.count()) m_iCurrentPoint = 0;
            return;

            LOG_DEBUG(QString("CTrajectory::processVehicle() : %1 reached waypoint, next = %2")
                      .arg(pObject->name())
                      .arg(m_iCurrentPoint));
        }

        double dSpeedMS = pObject->getSpeedMS();
        double dTurnSpeedDS = pObject->getTurnSpeedDS();

        CVector3 vRotation = pObject->getOriginRotation();
        double dCurrentAngleY = vRotation.Y;
        double dTargetAngleY = vDirection.AngleY();
        double dDiffAngleY = Math::Angles::angleDifferenceRadian(dTargetAngleY, dCurrentAngleY) * 2.0;

        if (dDiffAngleY >  1.0) dDiffAngleY =  1.0;
        if (dDiffAngleY < -1.0) dDiffAngleY = -1.0;

        vRotation.Y += (dDiffAngleY * Angles::toRad(dTurnSpeedDS)) * dDeltaTime;

        CAxis aLocalAxis(pObject->getECEFRotation());
        pObject->setOriginPosition(pObject->getOriginPosition() + aLocalAxis.Front * (dSpeedMS * dDeltaTime));
        pObject->setOriginRotation(vRotation);
    }
}
