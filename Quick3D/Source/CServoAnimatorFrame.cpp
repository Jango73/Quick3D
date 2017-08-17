
// Application
#include "C3DScene.h"
#include "CServoAnimatorFrame.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CServoAnimatorFrame::CServoAnimatorFrame()
{
}

//-------------------------------------------------------------------------------------------------

CServoAnimatorFrame::~CServoAnimatorFrame()
{
}

//-------------------------------------------------------------------------------------------------

void CServoAnimatorFrame::setAccelerationFactor(double dValue)
{
	m_tServo_X.setAccelerationFactor(dValue);
	m_tServo_Y.setAccelerationFactor(dValue);
	m_tServo_Z.setAccelerationFactor(dValue);
}

//-------------------------------------------------------------------------------------------------

void CServoAnimatorFrame::setVelocityFactor(double dValue)
{
	m_tServo_X.setVelocityFactor(dValue);
	m_tServo_Y.setVelocityFactor(dValue);
	m_tServo_Z.setVelocityFactor(dValue);
}

//-------------------------------------------------------------------------------------------------

CVector3 CServoAnimatorFrame::compute(double dDeltaTime, CVector3 vCurrentValue)
{
	m_vCurrentValue = vCurrentValue;

	if (m_vSteps.count() > 0)
	{
        // Are we in a waiting loop?
		if (m_dTime > 0.0)
		{
			m_dTime -= dDeltaTime;
		}
		else
		{
			if (m_vCurrentValue.isSame(m_vSteps[m_iCurrentStep].target()))
			{
				m_dTime = m_vSteps[m_iCurrentStep].duration();

				m_iCurrentStep++;

				if (m_iCurrentStep >= m_vSteps.count())
				{
					m_iCurrentStep = 0;
				}
			}

            // Get values for current step
			if (m_iCurrentStep < m_vSteps.count())
			{
				CVector3 vTarget = m_vSteps[m_iCurrentStep].target();
				CVector3 vSpeed = m_vSteps[m_iCurrentStep].speed();
				CVector3 vAccel = m_vSteps[m_iCurrentStep].acceleration();

				m_tServo_X.setMaximumAcceleration(vAccel.X);
				m_tServo_Y.setMaximumAcceleration(vAccel.Y);
				m_tServo_Z.setMaximumAcceleration(vAccel.Z);

				m_tServo_X.setMaximumVelocity(vSpeed.X);
				m_tServo_Y.setMaximumVelocity(vSpeed.Y);
				m_tServo_Z.setMaximumVelocity(vSpeed.Z);

				m_tServo_X.setTargetPosition(vTarget.X);
				m_tServo_Y.setTargetPosition(vTarget.Y);
				m_tServo_Z.setTargetPosition(vTarget.Z);

				m_tServo_X.setPosition(m_vCurrentValue.X);
				m_tServo_Y.setPosition(m_vCurrentValue.Y);
				m_tServo_Z.setPosition(m_vCurrentValue.Z);

				m_tServo_X.update(dDeltaTime);
				m_tServo_Y.update(dDeltaTime);
				m_tServo_Z.update(dDeltaTime);

				m_vCurrentValue.X = m_tServo_X.position();
				m_vCurrentValue.Y = m_tServo_Y.position();
				m_vCurrentValue.Z = m_tServo_Z.position();
			}
		}
	}

	return m_vCurrentValue;
}

//-------------------------------------------------------------------------------------------------

CVector3 CServoAnimatorFrame::getCurrentValue() const
{
	return m_vCurrentValue;
}
