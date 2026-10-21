
#pragma once

// Application
#include "CEnvironmentDefaults.h"
#include "CAtmosphereOptics.h"
#include "CVector3.h"

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

namespace CSunlightModel
{

/************************************************************************/
/*!
    Returns the direct sun color for \a dSunElevationSine, the sine of the sun
    elevation above the horizon. Callers compute it as the dot product of the
    normalized sun direction with the local up vector, which equals
    sin(elevation) and cos(zenith) at once. Values at or below zero render
    black, values above one are clamped.
*/
inline Math::CVector3 computeSunlightColor(double dSunElevationSine, double dOzoneAmount = EnvironmentDefaults::OzoneAmount)
{
    if (dSunElevationSine <= 0.0)
    {
        return Math::CVector3(0.0, 0.0, 0.0);
    }

    dSunElevationSine = CAtmosphereOptics::clampSinElevation(dSunElevationSine);
    dOzoneAmount = CAtmosphereOptics::clampOzoneAmount(dOzoneAmount);

    Math::CVector3 vTransmittance = CAtmosphereOptics::computeSunTransmittance(
        dSunElevationSine,
        AtmospherePhysics::RayleighScaleHeight,
        AtmospherePhysics::RayleighBetaR,
        AtmospherePhysics::RayleighBetaG,
        AtmospherePhysics::RayleighBetaB,
        AtmospherePhysics::OzoneBetaR,
        AtmospherePhysics::OzoneBetaG,
        AtmospherePhysics::OzoneBetaB,
        dOzoneAmount);

    return Math::CVector3(
        AtmospherePhysics::SunRadianceScale * vTransmittance.X,
        AtmospherePhysics::SunRadianceScale * vTransmittance.Y,
        AtmospherePhysics::SunRadianceScale * vTransmittance.Z);
}

} // namespace CSunlightModel
