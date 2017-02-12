
#include "CAtmosphere.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

CAtmosphere::CAtmosphere()
{
	m_Density_kgm3.addValue(    0.0,     1.26);
	m_Density_kgm3.addValue( 2000.0,     1.04);
	m_Density_kgm3.addValue( 4000.0,     0.84);
	m_Density_kgm3.addValue( 6000.0,     0.68);
	m_Density_kgm3.addValue( 8000.0,     0.52);
	m_Density_kgm3.addValue(10000.0,     0.40);
	m_Density_kgm3.addValue(12000.0,     0.32);
	m_Density_kgm3.addValue(15000.0,     0.20);
	m_Density_kgm3.addValue(18000.0,     0.12);
	m_Density_kgm3.addValue(19000.0,     0.10);
	m_Density_kgm3.addValue(20000.0,     0.09);
	m_Density_kgm3.addValue(22000.0,     0.06);
	m_Density_kgm3.addValue(25000.0,     0.04);
	m_Density_kgm3.addValue(30000.0,     0.02);
	m_Density_kgm3.addValue(40000.0,     0.00);

	m_Pressure_Nm2.addValue(    0.0, 106000.0);
	m_Pressure_Nm2.addValue( 1000.0,  92000.0);
	m_Pressure_Nm2.addValue( 5000.0,  54000.0);
	m_Pressure_Nm2.addValue( 7000.0,  40000.0);
	m_Pressure_Nm2.addValue( 9000.0,  30000.0);
	m_Pressure_Nm2.addValue(10000.0,  25000.0);
	m_Pressure_Nm2.addValue(11000.0,  22000.0);
	m_Pressure_Nm2.addValue(12000.0,  18000.0);
	m_Pressure_Nm2.addValue(15000.0,  12000.0);
	m_Pressure_Nm2.addValue(20000.0,   6000.0);
	m_Pressure_Nm2.addValue(25000.0,   2100.0);
	m_Pressure_Nm2.addValue(30000.0,    500.0);
	m_Pressure_Nm2.addValue(35000.0,      0.0);

	m_SoundSpeed_ms.addValue(    0.0, 340.0);
	m_SoundSpeed_ms.addValue(11000.0, 295.0);
	m_SoundSpeed_ms.addValue(20000.0, 295.0);
	m_SoundSpeed_ms.addValue(31000.0, 302.0);

	m_Temperature_K.addValue(    0.0, 288.0);
	m_Temperature_K.addValue(11000.0, 217.0);
	m_Temperature_K.addValue(20000.0, 217.0);
	m_Temperature_K.addValue(33000.0, 228.0);
}

//-------------------------------------------------------------------------------------------------

double CAtmosphere::density_kgm3(double dAltitude_m)
{
	return m_Density_kgm3.getValue(dAltitude_m);
}

//-------------------------------------------------------------------------------------------------

double CAtmosphere::pressure_Nm2(double dAltitude_m)
{
	return m_Pressure_Nm2.getValue(dAltitude_m);
}

//-------------------------------------------------------------------------------------------------

double CAtmosphere::soundSpeed_ms(double dAltitude_m)
{
	return m_SoundSpeed_ms.getValue(dAltitude_m);
}

//-------------------------------------------------------------------------------------------------

double CAtmosphere::temperature_K(double dAltitude_m)
{
	return m_Temperature_K.getValue(dAltitude_m);
}

//-------------------------------------------------------------------------------------------------

double CAtmosphere::airForceFactor(double dAltitude_m)
{
	double dAirDensity_kgm3 = density_kgm3(dAltitude_m);
	double dAirForceFactor = 1.0 / (1.0 + dAirDensity_kgm3);

	return dAirForceFactor;
}

//-------------------------------------------------------------------------------------------------

double CAtmosphere::airDragFactor(double dAltitude_m)
{
	double dAirDensity_kgm3 = density_kgm3(dAltitude_m);
	double dAirDragFactor = dAirDensity_kgm3;

	return dAirDragFactor;
}
