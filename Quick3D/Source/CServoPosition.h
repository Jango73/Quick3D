
#pragma once

#include "quick3d_global.h"

class QUICK3D_EXPORT CServoPosition
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par d�faut
    CServoPosition();

    //! Constructeur avec param�tres
    CServoPosition(double dMaximumAcceleration, double dMaximumVelocity, double dAccelerationFactor = 10.0, double dVelocityFactor = 2.0);

    //! Destructeur
    virtual ~CServoPosition();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! R�gle la position courante
    void setPosition(double value) { m_dPosition = value; }

    //! R�gle la position cible
    void setTargetPosition(double value) { m_dTargetPosition = value; }

    //! R�gle l'acc�l�ration maximum
    void setMaximumAcceleration(double value) { m_dMaximumAcceleration = value; }

    //! R�gle la vitesse maximum
    void setMaximumVelocity(double value) { m_dMaximumVelocity = value; }

    //! R�gle le facteur d'acc�l�ration
    void setAccelerationFactor(double value) { m_dAccelerationFactor = value; }

    //! R�gle le facteur de vitesse
    void setVelocityFactor(double value) { m_dVelocityFactor = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Retourne la position
    double position() const { return m_dPosition; }

    //! Retourne l'acceleration
    double acceleration() const { return m_dAcceleration; }

    //! Retourne la vitesse
    double velocity() const { return m_dVelocity; }

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //!
    CServoPosition& operator = (const CServoPosition& target);

    //!
    void update(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    double		m_dPosition;
    double		m_dTargetPosition;
    double		m_dMaximumAcceleration;
    double		m_dMaximumVelocity;
    double		m_dAccelerationFactor;
    double		m_dVelocityFactor;
    double		m_dAcceleration;
    double		m_dVelocity;
};
