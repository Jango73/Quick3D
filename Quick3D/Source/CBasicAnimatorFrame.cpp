
#include "C3DScene.h"
#include "CBasicAnimatorFrame.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CBasicAnimatorFrame::CBasicAnimatorFrame()
{
}

//-------------------------------------------------------------------------------------------------

CBasicAnimatorFrame::~CBasicAnimatorFrame()
{
}

//-------------------------------------------------------------------------------------------------

CVector3 CBasicAnimatorFrame::compute(double dDeltaTime, CVector3 vCurrentValue)
{
	m_vCurrentValue = vCurrentValue;

	if (m_vSteps.count() > 0)
	{
		// Est-on dans une boucle d'attente?
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

			if (m_iCurrentStep < m_vSteps.count())
			{
				CVector3 vTarget = m_vSteps[m_iCurrentStep].target();
				CVector3 vSpeed = m_vSteps[m_iCurrentStep].speed();

				m_vCurrentValue.X = computeSingleAxis(dDeltaTime, m_vCurrentValue.X, vTarget.X, vSpeed.X);
				m_vCurrentValue.Y = computeSingleAxis(dDeltaTime, m_vCurrentValue.Y, vTarget.Y, vSpeed.Y);
				m_vCurrentValue.Z = computeSingleAxis(dDeltaTime, m_vCurrentValue.Z, vTarget.Z, vSpeed.Z);
			}
		}
	}

	return m_vCurrentValue;
}

//-------------------------------------------------------------------------------------------------

CVector3 CBasicAnimatorFrame::getCurrentValue() const
{
	return m_vCurrentValue;
}

//-------------------------------------------------------------------------------------------------

double CBasicAnimatorFrame::computeSingleAxis(double dDeltaTime, double dCurrentValue, double dTargetValue, double dSpeed)
{
	if (dCurrentValue < dTargetValue)
	{
		dCurrentValue += dSpeed * dDeltaTime;

		if (dCurrentValue > dTargetValue) dCurrentValue = dTargetValue;
	}
	else
	if (dCurrentValue > dTargetValue)
	{
		dCurrentValue -= dSpeed * dDeltaTime;

		if (dCurrentValue < dTargetValue) dCurrentValue = dTargetValue;
	}

	return dCurrentValue;
}
