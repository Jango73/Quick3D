
// Application
#include "CServoPosition.h"
#include "Angles.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CServoPosition::CServoPosition()
: m_dPosition(0.0)
, m_dTargetPosition(0.0)
, m_dVelocity(0.0)
, m_dMaximumAcceleration(10.0)
, m_dMaximumVelocity(10.0)
, m_dAccelerationFactor(10.0)
, m_dVelocityFactor(2.0)
{
}

//-------------------------------------------------------------------------------------------------

CServoPosition::CServoPosition(double dMaximumAcceleration, double dMaximumVelocity, double dAccelerationFactor, double dVelocityFactor)
: m_dPosition(0.0)
, m_dTargetPosition(0.0)
, m_dVelocity(0.0)
, m_dMaximumAcceleration(dMaximumAcceleration)
, m_dMaximumVelocity(dMaximumVelocity)
, m_dAccelerationFactor(dAccelerationFactor)
, m_dVelocityFactor(dVelocityFactor)
{
}

//-------------------------------------------------------------------------------------------------

CServoPosition::~CServoPosition()
{
}

//-------------------------------------------------------------------------------------------------

CServoPosition& CServoPosition::operator = (const CServoPosition& target)
{
	m_dPosition = target.m_dPosition;
	m_dTargetPosition = target.m_dTargetPosition;
	m_dMaximumAcceleration = target.m_dMaximumAcceleration;
	m_dMaximumVelocity = target.m_dMaximumVelocity;
	m_dAccelerationFactor = target.m_dAccelerationFactor;
	m_dVelocityFactor = target.m_dVelocityFactor;
	m_dAcceleration = target.m_dAcceleration;
	m_dVelocity = target.m_dVelocity;

	return *this;
}

//-------------------------------------------------------------------------------------------------

void CServoPosition::update(double dDeltaTime)
{
	double dError = m_dTargetPosition - m_dPosition;
	double dVelocityDemand = Angles::clipDouble(dError * m_dVelocityFactor, -m_dMaximumVelocity, m_dMaximumVelocity);
	m_dAcceleration = Angles::clipDouble((dVelocityDemand - m_dVelocity) * m_dAccelerationFactor, -m_dMaximumAcceleration, m_dMaximumAcceleration);
	m_dVelocity = Angles::clipDouble(m_dVelocity + m_dAcceleration * dDeltaTime, -m_dMaximumVelocity, m_dMaximumVelocity);
	m_dPosition = m_dPosition + m_dVelocity * dDeltaTime;
}
