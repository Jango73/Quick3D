
#pragma once

//-------------------------------------------------------------------------------------------------
// Default rendering environment values.
//
// Every default used when a scene XML file omits the <Environment> node or one
// of its attributes lives here. Scene files override these values, engine code
// only reads them through C3DScene members, never as literals.
//-------------------------------------------------------------------------------------------------

#define DEFAULT_ATMOSPHERE_ALTITUDE         100000.0
#define DEFAULT_FOG_MINIMUM_DISTANCE        50.0
#define DEFAULT_UNDERWATER_FOG_DISTANCE     1000.0

#define DEFAULT_GLOBAL_AMBIENT_R            0.05
#define DEFAULT_GLOBAL_AMBIENT_G            0.05
#define DEFAULT_GLOBAL_AMBIENT_B            0.15

#define DEFAULT_SKY_COLOR_R                 0.2
#define DEFAULT_SKY_COLOR_G                 0.6
#define DEFAULT_SKY_COLOR_B                 1.0

#define DEFAULT_FOG_COLOR_R                 0.5
#define DEFAULT_FOG_COLOR_G                 0.5
#define DEFAULT_FOG_COLOR_B                 0.5

//-------------------------------------------------------------------------------------------------
// Underwater light absorption (Beer-Lambert law).
//
// Downwelling light decays exponentially with depth, independently per channel:
// transmitted = surface * exp(-Kd * depth). Red disappears in the first meters,
// blue persists down to about 200 meters, so the blue tint under water emerges
// from the computation instead of a forced color. Values are diffuse attenuation
// coefficients in m-1 for clear oceanic water (Jerlov type I).
//-------------------------------------------------------------------------------------------------

#define DEFAULT_WATER_ATTENUATION_R         0.33
#define DEFAULT_WATER_ATTENUATION_G         0.05
#define DEFAULT_WATER_ATTENUATION_B         0.02

//-------------------------------------------------------------------------------------------------
// Direct sunlight tint (Rayleigh scattering and ozone absorption).
//
// Surviving sunlight after crossing the atmosphere is computed as
// tint = scale * exp(-(beta + ozoneAmount * ozoneBeta) * path), with path =
// scale height * airmass.
// Scattering removes blue faster than red, so a low sun turns orange then red
// while a high sun stays warm white. Beta values are sea level Rayleigh
// scattering coefficients in m-1. The radiance scale maps the physical
// transmittance (0..1) to engine light units, tuned so noon matches the
// previous look.
//-------------------------------------------------------------------------------------------------

#define DEFAULT_RAYLEIGH_BETA_R             5.8e-6
#define DEFAULT_RAYLEIGH_BETA_G             13.5e-6
#define DEFAULT_RAYLEIGH_BETA_B             33.1e-6
#define DEFAULT_RAYLEIGH_SCALE_HEIGHT       7994.0
#define DEFAULT_SUN_RADIANCE_SCALE          1.46

//-------------------------------------------------------------------------------------------------
// Ozone absorption (Chappuis bands).
//
// Effective RGB-fitted ozone extinction in m-1, applied over the same analytic
// slant path as Rayleigh scattering. The green channel absorbs the most since
// the Chappuis peak near 600 nm falls between the green and red channels,
// which removes the green horizon cast of a pure Rayleigh sky. The scene ozone
// amount scales these values, 1.0 is the nominal column, 0.0 disables ozone.
//-------------------------------------------------------------------------------------------------

#define DEFAULT_OZONE_BETA_R                2.0e-6
#define DEFAULT_OZONE_BETA_G                5.0e-6
#define DEFAULT_OZONE_BETA_B                0.5e-6
#define DEFAULT_OZONE_AMOUNT                1.0

//-------------------------------------------------------------------------------------------------
// Sky rendering model.
//
// The scene XML selects the sky model by name. Only one model exists for now,
// additional models add their own name, id and shader branch here.
//-------------------------------------------------------------------------------------------------

#define SKY_MODEL_PHYSICAL                  "Physical"
#define SKY_MODEL_PHYSICAL_ID               0
#define DEFAULT_SKY_MODEL                   SKY_MODEL_PHYSICAL

//-------------------------------------------------------------------------------------------------
// Sky display exposure.
//
// The physical sky outputs small radiance values. This exposure maps them to
// display range, calibrated so the noon zenith matches the previous look.
//-------------------------------------------------------------------------------------------------

#define DEFAULT_SKY_EXPOSURE                32.0
