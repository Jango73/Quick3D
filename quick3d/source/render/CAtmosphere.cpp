
#include "CAtmosphere.h"

//-------------------------------------------------------------------------------------------------

CAtmosphere::CAtmosphere()
{
	// Density cubic (y=ax^3+bx^2+cx+d)
	// a 	-4.584e-14
	// b	4.032e-09
	// c	-1.211e-04
	// d	1.263e+00

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

	// Pressure cubic (y=ax^3+bx^2+cx+d)
	// a	105378.3
	// b	−13.13734
	// c	0.0006169831
	// d	−1.043082*10−8

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

/*
Function calculateAtmosphericPressure(altitudeInMeters, seaLevelPressureInPascal, seaLevelTemperatureInCelsius)
    // Inputs:
    //   altitudeInMeters : Float (e.g., 2500.0)
    //   seaLevelPressureInPascal : Float (Standard is 101325.0 N/m² or Pa)
    //   seaLevelTemperatureInCelsius : Float (Standard is 15.0 °C)
    // Output:
    //   Atmospheric pressure in Newtons per square meter (N/m² or Pascal)

    // Constants
    g = 9.80665          // Acceleration due to gravity (m/s²)
    M = 0.0289644        // Molar mass of Earth's air (kg/mol)
    R = 8.3144598        // Universal gas constant (J/(mol·K))
    L = 0.0065           // Temperature lapse rate in troposphere (K/m)

    // 1. Convert sea level temperature to Kelvin
    T0 = seaLevelTemperatureInCelsius + 273.15

    // 2. Calculate temperature at the given altitude
    temperatureAtAltitude = T0 - (L * altitudeInMeters)

    // 3. Prevent division by zero or negative temperatures at extreme altitudes
    If temperatureAtAltitude <= 0 Then
        Return 0.0
    EndIf

    // 4. Calculate the exponent component (g * M) / (R * L)
    exponent = (g * M) / (R * L) // This evaluates to approximately 5.25588

    // 5. Apply the barometric formula
    pressure = seaLevelPressureInPascal * pow((temperatureAtAltitude / T0), exponent)

    Return pressure
EndFunction
*/

//-------------------------------------------------------------------------------------------------

double CAtmosphere::soundSpeed_ms(double dAltitude_m)
{
	return m_SoundSpeed_ms.getValue(dAltitude_m);
}

/*
Function calculateSpeedOfSound(temperatureInCelsius, relativeHumidityInPercent)
    // Inputs:
    //   temperatureInCelsius : Float (e.g., 20.0)
    //   relativeHumidityInPercent : Float (e.g., 50.0 for 50%)
    // Output:
    //   Speed of sound in meters per second (Float)

    // 1. Convert humidity percentage to a fraction (e.g., 50% becomes 0.5)
    humidityFraction = relativeHumidityInPercent / 100.0

    // 2. Calculate the base speed based on temperature alone
    temperatureEffect = 331.45 + (0.606 * temperatureInCelsius)

    // 3. Calculate the moisture effect using the exponential function 'exp'
    humidityEffect = 0.0124 * humidityFraction * exp(0.06 * temperatureInCelsius)

    // 4. Sum both components to get the final velocity
    speedOfSound = temperatureEffect + humidityEffect

    Return speedOfSound
EndFunction
*/

//-------------------------------------------------------------------------------------------------

double CAtmosphere::temperature_K(double dAltitude_m)
{
	return m_Temperature_K.getValue(dAltitude_m);
}

/*
Function calculateTemperatureAtAltitude(altitudeInMeters, seaLevelTemperatureInCelsius)
    // Inputs:
    //   altitudeInMeters : Float (e.g., 12000.0)
    //   seaLevelTemperatureInCelsius : Float (Standard is 15.0 °C)
    // Output:
    //   Air temperature at the given altitude in Celsius (Float)

    // Constants for atmospheric layers
    TROPOPAUSE_ALTITUDE = 11000.0      // Boundary between Troposphere and Stratosphere (m)
    STRATOSPHERE_UPPER = 20000.0       // Lower Stratosphere boundary (m)
    
    LAPSE_RATE_TROPO = 0.0065          // Temperature drop in Troposphere (6.5°C per 1000m)
    LAPSE_RATE_STRATO = -0.0010        // Temperature rise in Mid-Stratosphere (-1.0°C per 1000m)

    // 1. Layer 1: Troposphere (0 to 11,000 meters)
    If altitudeInMeters <= TROPOPAUSE_ALTITUDE Then
        temperature = seaLevelTemperatureInCelsius - (LAPSE_RATE_TROPO * altitudeInMeters)
        Return temperature
    EndIf

    // Calculate temperature at the exact boundary of the tropopause
    tempAtTropopause = seaLevelTemperatureInCelsius - (LAPSE_RATE_TROPO * TROPOPAUSE_ALTITUDE)

    // 2. Layer 2: Lower Stratosphere / Tropopause (11,000 to 20,000 meters)
    // Temperature remains perfectly constant here (approx -56.5°C if sea level is 15°C)
    If altitudeInMeters <= STRATOSPHERE_UPPER Then
        Return tempAtTropopause
    EndIf

    // 3. Layer 3: Mid-Stratosphere (20,000 to 32,000 meters)
    // Temperature starts rising again due to ozone UV absorption
    altitudeInStrato = altitudeInMeters - STRATOSPHERE_UPPER
    temperature = tempAtTropopause - (LAPSE_RATE_STRATO * altitudeInStrato)
    
    Return temperature
EndFunction
*/

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
