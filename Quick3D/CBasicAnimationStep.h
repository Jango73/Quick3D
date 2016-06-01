
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

	//! Constructeur par défaut
	CBasicAnimationStep();

	//! Constructeur avec paramètres
	CBasicAnimationStep(Math::CVector3 vTarget, Math::CVector3 vSpeed, Math::CVector3 vAcceleration, double dDuration);

	//! Destructeur
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

	//! Retourne la cible (translation ou rotation)
	Math::CVector3 getTarget() const { return m_vTarget; }

	//! Retourne la vitesse (translation ou rotation)
	Math::CVector3 getSpeed() const { return m_vSpeed; }

	//! Retourne l'accélération (translation ou rotation)
	Math::CVector3 getAcceleration() const { return m_vAcceleration; }

	//! Retourne la durée en secondes
	double getDuration() const { return m_dDuration; }

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	CBasicAnimationStep& operator = (const CBasicAnimationStep& target);

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	Math::CVector3	m_vTarget;
	Math::CVector3	m_vSpeed;
	Math::CVector3	m_vAcceleration;
	double			m_dDuration;
};
