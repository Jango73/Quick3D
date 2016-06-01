
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector2.h"
#include "CVector3.h"

class QUICK3D_EXPORT CFog
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CFog();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	bool enabled() const { return m_bEnabled; }

	//!
	bool& enabled() { return m_bEnabled; }

	//!
	double level() const { return m_dLevel; }

	//!
	double& level() { return m_dLevel; }

	//!
	double distance() const { return m_dDistance; }

	//!
	double& distance() { return m_dDistance; }

	//!
	Math::CVector3& color() { return m_vColor; }

	//!
	Math::CVector3 color() const { return m_vColor; }

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	bool			m_bEnabled;
	double			m_dLevel;
	double			m_dDistance;
	Math::CVector3	m_vColor;
};
