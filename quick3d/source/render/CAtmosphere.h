
#pragma once

#include "quick3d_global.h"

// qt-plus
#include "CSingleton.h"

// Application
#include "CInterpolator.h"

#define N_TO_KG	(0.101971621)

class QUICK3D_EXPORT CAtmosphere : public CSingleton<CAtmosphere>
{
    friend class CSingleton<CAtmosphere>;

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CAtmosphere();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Gets the density in kilograms per cubic meter for a specified altitude
    double density_kgm3(double dAltitude_m);

    //! Gets the pressure in newtons per square meter for a specified altitude
    double pressure_Nm2(double dAltitude_m);

    //! Gets the speed of sound in meters per second for a specified altitude
    double soundSpeed_ms(double dAltitude_m);

    //! Gets the temperature of air in kelvins for a specified altitude
    double temperature_K(double dAltitude_m);

    //! Gets an air force factor for a specified altitude
    //! Used for physics computations
    double airForceFactor(double dAltitude_m);

    //! Gets an air drag factor for a specified altitude
    //! Used for physics computations
    double airDragFactor(double dAltitude_m);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CInterpolator<double>   m_Density_kgm3;
    CInterpolator<double>   m_Pressure_Nm2;
    CInterpolator<double>   m_SoundSpeed_ms;
    CInterpolator<double>   m_Temperature_K;
};
