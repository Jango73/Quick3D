
#pragma once

// C++
#include <cmath>

// Application
#include "CEnvironmentDefaults.h"
#include "CAtmosphereOptics.h"
#include "CVector3.h"
#include "Angles.h"

//-------------------------------------------------------------------------------------------------
// Physical sunlight tint.
//
// Computes the direct sun color from the sine of the sun elevation using
// Rayleigh scattering and ozone absorption: surviving = scale * exp(-ext * path),
// with ext = beta + ozoneAmount * ozoneBeta and the relative airmass from
// Kasten and Young (1989). A high sun renders warm white, a low sun turns
// orange then red, a sun below the horizon renders black. Replaces the former
// keyframed tint table.
//-------------------------------------------------------------------------------------------------

inline Math::CVector3 computeSunlightColor(double dSinElevation, double dOzoneAmount = DEFAULT_OZONE_AMOUNT)
{
    if (dSinElevation <= 0.0)
    {
        return Math::CVector3(0.0, 0.0, 0.0);
    }

    if (dSinElevation > 1.0)
    {
        dSinElevation = 1.0;
    }

    Math::CVector3 vTransmittance = CAtmosphereOptics::computeSunTransmittance(
        dSinElevation,
        DEFAULT_RAYLEIGH_SCALE_HEIGHT,
        DEFAULT_RAYLEIGH_BETA_R,
        DEFAULT_RAYLEIGH_BETA_G,
        DEFAULT_RAYLEIGH_BETA_B,
        DEFAULT_OZONE_BETA_R,
        DEFAULT_OZONE_BETA_G,
        DEFAULT_OZONE_BETA_B,
        dOzoneAmount);

    return Math::CVector3(
        DEFAULT_SUN_RADIANCE_SCALE * vTransmittance.X,
        DEFAULT_SUN_RADIANCE_SCALE * vTransmittance.Y,
        DEFAULT_SUN_RADIANCE_SCALE * vTransmittance.Z);
}
