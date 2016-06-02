
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
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur par défaut
	CServoAnimatorFrame();

	//! Destructeur
	virtual ~CServoAnimatorFrame();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//! Règle le facteur d'accélération
	void setAccelerationFactor(double dValue);

	//! Règle le facteur de vitesse
	void setVelocityFactor(double value);

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//! Calcule une valeur cible selon une valeur courante
	virtual Math::CVector3 compute(double dDeltaTime, Math::CVector3 vCurrentValue);

	//!
	virtual Math::CVector3 getCurrentValue() const;

	//-------------------------------------------------------------------------------------------------
	// Méthodes protégées
	//-------------------------------------------------------------------------------------------------

protected:

	//! Calcule une valeur cible selon une valeur courante
	double computeSingleAxis(double dDeltaTime, double dCurrentValue, double dTargetValue, double dSpeed);

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	Math::CVector3	m_vCurrentValue;
	CServoPosition	m_tServo_X;
	CServoPosition	m_tServo_Y;
	CServoPosition	m_tServo_Z;
};
