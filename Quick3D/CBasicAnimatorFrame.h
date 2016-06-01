
#pragma once

#include "quick3d_global.h"

// Qt
#include <QVector>

// Application
#include "CQ3DConstants.h"
#include "CAnimatorFrame.h"

class QUICK3D_EXPORT CBasicAnimatorFrame : public CAnimatorFrame
{

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur par d�faut
	CBasicAnimatorFrame();

	//! Destructeur
	virtual ~CBasicAnimatorFrame();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//! Calcule une valeur cible selon une valeur courante
	virtual Math::CVector3 compute(double dDeltaTime, Math::CVector3 vCurrentValue);

	//!
	virtual Math::CVector3 getCurrentValue() const;

	//-------------------------------------------------------------------------------------------------
	// M�thodes prot�g�es
	//-------------------------------------------------------------------------------------------------

protected:

	//! Calcule une valeur cible selon une valeur courante
	double computeSingleAxis(double dDeltaTime, double dCurrentValue, double dTargetValue, double dSpeed);

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	Math::CVector3	m_vCurrentValue;
};
