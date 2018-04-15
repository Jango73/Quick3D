
#include "CBasicAnimationStep.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CBasicAnimationStep::CBasicAnimationStep()
    : m_dDuration(0.0)
{
}

//-------------------------------------------------------------------------------------------------

CBasicAnimationStep::CBasicAnimationStep(CVector3 vTarget, CVector3 vSpeed, Math::CVector3 vAcceleration, double dDuration)
    : m_vTarget(vTarget)
    , m_vSpeed(vSpeed)
    , m_vAcceleration(vAcceleration)
    , m_dDuration(dDuration)
{
}

//-------------------------------------------------------------------------------------------------

CBasicAnimationStep::~CBasicAnimationStep()
{
}

//-------------------------------------------------------------------------------------------------

CBasicAnimationStep& CBasicAnimationStep::operator = (const CBasicAnimationStep& target)
{
    m_vTarget = target.m_vTarget;
    m_vSpeed = target.m_vSpeed;
    m_dDuration = target.m_dDuration;
    m_vAcceleration = target.m_vAcceleration;

    return *this;
}
