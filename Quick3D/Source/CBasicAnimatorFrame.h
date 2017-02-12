
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
	// Constructors and destructor
	//-------------------------------------------------------------------------------------------------

	//! Default constructor
	CBasicAnimatorFrame();

	//! Destructor
	virtual ~CBasicAnimatorFrame();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Control methods
	//-------------------------------------------------------------------------------------------------

	//! Calcule une valeur cible selon une valeur courante
	virtual Math::CVector3 compute(double dDeltaTime, Math::CVector3 vCurrentValue);

	//!
	virtual Math::CVector3 getCurrentValue() const;

	//-------------------------------------------------------------------------------------------------
	// Protected methods
	//-------------------------------------------------------------------------------------------------

protected:

	//! Calcule une valeur cible selon une valeur courante
	double computeSingleAxis(double dDeltaTime, double dCurrentValue, double dTargetValue, double dSpeed);

	//-------------------------------------------------------------------------------------------------
	// Properties
	//-------------------------------------------------------------------------------------------------

protected:

	Math::CVector3	m_vCurrentValue;
};
