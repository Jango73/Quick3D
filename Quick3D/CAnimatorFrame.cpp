
#include "C3DScene.h"
#include "CAnimatorFrame.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CAnimatorFrame::CAnimatorFrame()
: m_iCurrentStep(0)
, m_dTime(0.0)
{
}

//-------------------------------------------------------------------------------------------------

CAnimatorFrame::~CAnimatorFrame()
{
}

//-------------------------------------------------------------------------------------------------

void CAnimatorFrame::addStep(CBasicAnimationStep value)
{
	m_vSteps.append(value);
}

//-------------------------------------------------------------------------------------------------

Math::CVector3 CAnimatorFrame::compute(double dDeltaTime, Math::CVector3 vCurrentValue)
{
	return CVector3();
}

//-------------------------------------------------------------------------------------------------

CVector3 CAnimatorFrame::getCurrentValue() const
{
	return CVector3();
}
