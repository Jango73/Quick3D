
#pragma once

#include "quick3d_global.h"

class QUICK3D_EXPORT CServoPosition
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par défaut
    CServoPosition();

    //! Constructeur avec paramètres
    CServoPosition(double dMaximumAcceleration, double dMaximumVelocity, double dAccelerationFactor = 10.0, double dVelocityFactor = 2.0);

    //! Destructeur
    virtual ~CServoPosition();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Règle la position courante
    void setPosition(double value) { m_dPosition = value; }

    //! Règle la position cible
    void setTargetPosition(double value) { m_dTargetPosition = value; }

    //! Règle l'accélération maximum
    void setMaximumAcceleration(double value) { m_dMaximumAcceleration = value; }

    //! Règle la vitesse maximum
    void setMaximumVelocity(double value) { m_dMaximumVelocity = value; }

    //! Règle le facteur d'accélération
    void setAccelerationFactor(double value) { m_dAccelerationFactor = value; }

    //! Règle le facteur de vitesse
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
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    CServoPosition& operator = (const CServoPosition& target);

    //!
    void update(double dDeltaTime);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
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
