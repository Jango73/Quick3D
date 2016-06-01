
#pragma once

#include "quick3d_global.h"

// Qt
#include <QString>
#include <QVector>

// Fondations
#include "CVector3.h"
#include "CGeoloc.h"

class CTrajectorable;

class QUICK3D_EXPORT CTrajectory
{

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CTrajectory(bool bAutoOrientation = true);

	//!
	virtual ~CTrajectory();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setOriginPosition(Math::CGeoloc Position);

	//!
	void setOriginRotation(Math::CVector3 Rotation);

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	Math::CGeoloc getOriginPosition();

	//!
	Math::CVector3 getOriginRotation();

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	void reset();

	//!
	void addPoint(Math::CGeoloc vPoint);

	//!
	void processObject(CTrajectorable* pVehicle, double dDeltaTime);

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	Math::CGeoloc			m_gOriginPosition;
	Math::CVector3			m_vOriginRotation;
	QVector<Math::CGeoloc>	m_vPoints;
	int						m_iCurrentPoint;
	bool					m_bAutoOrientation;
};
