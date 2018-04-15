
#include "C3DScene.h"
#include "CAnimatorFrame.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    \class CAnimatorFrame
    \brief The base class for an animator frame.
    \inmodule Quick3D
    \sa CComponent
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CAnimator with its default parameters.
*/
CAnimatorFrame::CAnimatorFrame()
    : m_iCurrentStep(0)
    , m_dTime(0.0)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CAnimatorFrame.
*/
CAnimatorFrame::~CAnimatorFrame()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds an animation step defined by \a value.
*/
void CAnimatorFrame::addStep(CBasicAnimationStep value)
{
    m_vSteps.append(value);
}

//-------------------------------------------------------------------------------------------------

/*!
    Computes a target value. \br\br
    \a dDeltaTime is the elapsed time in seconds since the last frame. \br
    \a vCurrentValue is the current value.
*/
Math::CVector3 CAnimatorFrame::compute(double dDeltaTime, Math::CVector3 vCurrentValue)
{
    return CVector3();
}

//-------------------------------------------------------------------------------------------------

CVector3 CAnimatorFrame::currentValue() const
{
    return CVector3();
}
