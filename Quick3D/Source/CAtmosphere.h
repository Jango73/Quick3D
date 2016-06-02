
#pragma once

#include "quick3d_global.h"

// Application
#include "CSingleton.h"
#include "Interpolator.h"

#define N_TO_KG	(0.101971621)

class QUICK3D_EXPORT CAtmosphere : public CSingleton<CAtmosphere>
{
	friend class CSingleton<CAtmosphere>;

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CAtmosphere();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	double getDensity_kgm3(double dAltitude_m);

	//!
	double getPressure_Nm2(double dAltitude_m);

	//!
	double getSoundSpeed_ms(double dAltitude_m);

	//!
	double getTemperature_K(double dAltitude_m);

	//!
	double getAirForceFactor(double dAltitude_m);

	//!
	double getAirDragFactor(double dAltitude_m);

	//-------------------------------------------------------------------------------------------------
	// Propriétés
	//-------------------------------------------------------------------------------------------------

protected:

	Math::Interpolator<double>	m_Density_kgm3;
	Math::Interpolator<double>	m_Pressure_Nm2;
	Math::Interpolator<double>	m_SoundSpeed_ms;
	Math::Interpolator<double>	m_Temperature_K;
};
