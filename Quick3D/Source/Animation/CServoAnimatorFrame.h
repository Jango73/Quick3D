
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>

// Application
#include "CQ3DConstants.h"
#include "CAnimatorFrame.h"
#include "CServoPosition.h"

class QUICK3D_EXPORT CServoAnimatorFrame : public CAnimatorFrame
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CServoAnimatorFrame();

    //! Destructor
    virtual ~CServoAnimatorFrame();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets the acceleration factor
    void setAccelerationFactor(double dValue);

    //! Sets the velocity factor
    void setVelocityFactor(double value);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Computes a target value given a current value and a delta time
    virtual Math::CVector3 compute(double dDeltaTime, Math::CVector3 vCurrentValue) Q_DECL_OVERRIDE;

    //! Returns the current value
    virtual Math::CVector3 currentValue() const Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //! Computes the value on a single axis
    double computeSingleAxis(double dDeltaTime, double dCurrentValue, double dTargetValue, double dSpeed);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    Math::CVector3      m_vCurrentValue;
    CServoPosition      m_tServo_X;
    CServoPosition      m_tServo_Y;
    CServoPosition      m_tServo_Z;
};
