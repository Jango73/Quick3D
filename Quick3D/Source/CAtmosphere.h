
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CSingleton.h"

// Quick3D
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

    //!
    double density_kgm3(double dAltitude_m);

    //!
    double pressure_Nm2(double dAltitude_m);

    //!
    double soundSpeed_ms(double dAltitude_m);

    //!
    double temperature_K(double dAltitude_m);

    //!
    double airForceFactor(double dAltitude_m);

    //!
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
