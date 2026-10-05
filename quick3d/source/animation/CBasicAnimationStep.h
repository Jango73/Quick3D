
#pragma once

#include "quick3d_global.h"

// Foundations
#include "CVector3.h"

class QUICK3D_EXPORT CBasicAnimationStep
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CBasicAnimationStep();

    //! Constructor with parameters
    CBasicAnimationStep(Math::CVector3 vTarget, Math::CVector3 vSpeed, Math::CVector3 vAcceleration, double dDuration);

    //! Destructor
    virtual ~CBasicAnimationStep();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Règle la cible (translation ou rotation)
    void setTarget(const Math::CVector3& value) { m_vTarget = value; }

    //! Règle la vitesse (translation ou rotation)
    void setSpeed(const Math::CVector3& value) { m_vSpeed = value; }

    //! Règle l'accélération (translation ou rotation)
    void setAcceleration(const Math::CVector3& value) { m_vAcceleration = value; }

    //! Règle la durée en secondes
    void setDuration(double value) { m_dDuration = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Returns the target (translation or rotation)
    Math::CVector3 target() const { return m_vTarget; }

    //! Returns the velocity (translation or rotation)
    Math::CVector3 speed() const { return m_vSpeed; }

    //! Returns the acceleration (translation or rotation)
    Math::CVector3 acceleration() const { return m_vAcceleration; }

    //! Returns the duration in seconds
    double duration() const { return m_dDuration; }

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    CBasicAnimationStep& operator = (const CBasicAnimationStep& target);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    Math::CVector3  m_vTarget;
    Math::CVector3  m_vSpeed;
    Math::CVector3  m_vAcceleration;
    double          m_dDuration;
};
