
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"

class QUICK3D_EXPORT CBasicAnimationStep
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par d�faut
    CBasicAnimationStep();

    //! Constructeur avec param�tres
    CBasicAnimationStep(Math::CVector3 vTarget, Math::CVector3 vSpeed, Math::CVector3 vAcceleration, double dDuration);

    //! Destructeur
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
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //!
    CBasicAnimationStep& operator = (const CBasicAnimationStep& target);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    Math::CVector3  m_vTarget;
    Math::CVector3  m_vSpeed;
    Math::CVector3  m_vAcceleration;
    double          m_dDuration;
};
