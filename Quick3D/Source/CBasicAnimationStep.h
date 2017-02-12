
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"

class QUICK3D_EXPORT CBasicAnimationStep
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CBasicAnimationStep();

    //! Constructeur avec param�tres
    CBasicAnimationStep(Math::CVector3 vTarget, Math::CVector3 vSpeed, Math::CVector3 vAcceleration, double dDuration);

    //! Destructor
    virtual ~CBasicAnimationStep();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! R�gle la cible (translation ou rotation)
    void setTarget(const Math::CVector3& value) { m_vTarget = value; }

    //! R�gle la vitesse (translation ou rotation)
    void setSpeed(const Math::CVector3& value) { m_vSpeed = value; }

    //! R�gle l'acc�l�ration (translation ou rotation)
    void setAcceleration(const Math::CVector3& value) { m_vAcceleration = value; }

    //! R�gle la dur�e en secondes
    void setDuration(double value) { m_dDuration = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne la cible (translation ou rotation)
    Math::CVector3 target() const { return m_vTarget; }

    //! Retourne la vitesse (translation ou rotation)
    Math::CVector3 speed() const { return m_vSpeed; }

    //! Retourne l'acc�l�ration (translation ou rotation)
    Math::CVector3 acceleration() const { return m_vAcceleration; }

    //! Retourne la dur�e en secondes
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
