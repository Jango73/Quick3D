
#pragma once

#include "quick3d_global.h"

class QUICK3D_EXPORT CServoPosition
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CServoPosition();

    //! Constructor with parameters
    CServoPosition(double dMaximumAcceleration, double dMaximumVelocity, double dAccelerationFactor = 10.0, double dVelocityFactor = 2.0);

    //! Destructor
    virtual ~CServoPosition();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets the current position
    void setPosition(double value) { m_dPosition = value; }

    //! Sets the target position
    void setTargetPosition(double value) { m_dTargetPosition = value; }

    //! Sets the maximum acceleration
    void setMaximumAcceleration(double value) { m_dMaximumAcceleration = value; }

    //! Sets the maximum speed
    void setMaximumVelocity(double value) { m_dMaximumVelocity = value; }

    //! Sets the acceleratio factor
    void setAccelerationFactor(double value) { m_dAccelerationFactor = value; }

    //! Sets the speed factor
    void setVelocityFactor(double value) { m_dVelocityFactor = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the position
    double position() const { return m_dPosition; }

    //! Returns the acceleration
    double acceleration() const { return m_dAcceleration; }

    //! Returns the speed
    double velocity() const { return m_dVelocity; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CServoPosition& operator = (const CServoPosition& target);

    //!
    void update(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    double      m_dPosition;
    double      m_dTargetPosition;
    double      m_dMaximumAcceleration;
    double      m_dMaximumVelocity;
    double      m_dAccelerationFactor;
    double      m_dVelocityFactor;
    double      m_dAcceleration;
    double      m_dVelocity;
};
