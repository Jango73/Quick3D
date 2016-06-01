
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

	//! Constructeur par d�faut
	CAnimatorFrame();

	//! Destructeur
	virtual ~CAnimatorFrame();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//! Retourne une r�f�rence sur la liste de pas
	QVector<CBasicAnimationStep>& getSteps() { return m_vSteps; }

	//! Retourne une r�f�rence sur la liste de pas
	const QVector<CBasicAnimationStep>& getSteps() const { return m_vSteps; }

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//! Ajoute un pas de rotation
	void addStep(CBasicAnimationStep value);

	//! Calcule une valeur cible selon une valeur courante
	virtual Math::CVector3 compute(double dDeltaTime, Math::CVector3 vCurrentValue);

	//!
	virtual Math::CVector3 getCurrentValue() const;

	//-------------------------------------------------------------------------------------------------
	// M�thodes prot�g�es
	//-------------------------------------------------------------------------------------------------

protected:

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	QVector<CBasicAnimationStep>	m_vSteps;
	int								m_iCurrentStep;
	double							m_dTime;
};
