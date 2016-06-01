
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CAverager.h"
#include "CLogManager.h"

// Application
#include "CQ3DConstants.h"
#include "CMesh.h"
#include "CTrajectory.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTrajectorable : public CMesh
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//! Constructeur
	CTrajectorable(C3DScene* pScene);

	//!
	virtual ~CTrajectorable();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//!
	void setSpeedMS(double value) { m_dSpeedMS = value; }

	//!
	void setTurnSpeedDS(double value) { m_dTurnSpeedDS = value; }

	//!
	void setTrajectoryEnabled(bool value) { m_bTrajectoryEnabled = value; }

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	double getSpeedMS() const { return m_dSpeedMS; }

	//!
	double getTurnSpeedDS() const { return m_dTurnSpeedDS; }

	//!
	CTrajectory& getTrajectory() { return m_tTrajectory; }

	//-------------------------------------------------------------------------------------------------
	// Méthodes héritées
	//-------------------------------------------------------------------------------------------------

	//!
	virtual QString getClassName() const { return ClassName_CTrajectorable; }

	//!
	virtual void update(double dDeltaTime);

	//!
	virtual bool isTrajectorable() const { return true; }

	//-------------------------------------------------------------------------------------------------
	// Méthodes de contrôle
	//-------------------------------------------------------------------------------------------------

	//!
	void resetTrajectory();

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	bool						m_bTrajectoryEnabled;
	double						m_dSpeedMS;
	double						m_dTurnSpeedDS;
	CTrajectory					m_tTrajectory;
	CAverager<Math::CVector3>	m_aRotation;
};
