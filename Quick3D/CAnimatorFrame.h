
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>

// Application
#include "CQ3DConstants.h"
#include "CBasicAnimationStep.h"

class QUICK3D_EXPORT CAnimatorFrame
{

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur par défaut
	CAnimatorFrame();

	//! Destructeur
	virtual ~CAnimatorFrame();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Retourne une référence sur la liste de pas
	QVector<CBasicAnimationStep>& getSteps() { return m_vSteps; }

	//! Retourne une référence sur la liste de pas
	const QVector<CBasicAnimationStep>& getSteps() const { return m_vSteps; }

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//! Ajoute un pas de rotation
	void addStep(CBasicAnimationStep value);

	//! Calcule une valeur cible selon une valeur courante
	virtual Math::CVector3 compute(double dDeltaTime, Math::CVector3 vCurrentValue);

	//!
	virtual Math::CVector3 getCurrentValue() const;

	//-------------------------------------------------------------------------------------------------
	// Méthodes protégées
	//-------------------------------------------------------------------------------------------------

protected:

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	QVector<CBasicAnimationStep>	m_vSteps;
	int								m_iCurrentStep;
	double							m_dTime;
};
