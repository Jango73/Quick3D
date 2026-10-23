
#pragma once

//-------------------------------------------------------------------------------------------------
// Default rendering environment values.
//
// Every default used when a scene XML file omits the <Environment> node or one
// of its attributes lives here. Scene files override these values, engine code
// only reads them through CEnvironment, never as literals.
//
// Physics (AtmospherePhysics) and configuration (EnvironmentDefaults) stay in
// separate namespaces so a tuning change cannot be mistaken for a physics
// change. Sky model identity (SkyModels) and shader uniform names
// (SkyUniforms) are shared by the C++ models and the GLSL sky code.
//-------------------------------------------------------------------------------------------------

namespace AtmospherePhysics
{

//-------------------------------------------------------------------------------------------------
// Rayleigh scattering and ozone absorption.
//-------------------------------------------------------------------------------------------------

// Sea level Rayleigh scattering coefficients in m-1
constexpr double RayleighBetaR = 5.8e-6;
constexpr double RayleighBetaG = 13.5e-6;
constexpr double RayleighBetaB = 33.1e-6;
constexpr double RayleighScaleHeight = 7994.0;
constexpr double SunRadianceScale = 1.46;

// Kasten and Young (1989) relative airmass coefficients. Single source of
// truth shared by CAtmosphereOptics and the GLSL sky include, keep the GLSL
// copy in sync when editing here
constexpr double KastenYoungA = 0.50572;
constexpr double KastenYoungB = 6.07995;
constexpr double KastenYoungC = 1.6364;

// Effective RGB-fitted ozone extinction in m-1, applied over the same analytic
// slant path as Rayleigh scattering. The green channel absorbs the most since
// the Chappuis peak near 600 nm falls between the green and red channels,
// which removes the green horizon cast of a pure Rayleigh sky
constexpr double OzoneBetaR = 2.0e-6;
constexpr double OzoneBetaG = 5.0e-6;
constexpr double OzoneBetaB = 0.5e-6;

// Underwater light absorption (Beer-Lambert law). Downwelling light decays
// exponentially with depth, independently per channel: transmitted = surface *
// exp(-Kd * depth). Red disappears in the first meters, blue persists down to
// about 200 meters, so the blue tint under water emerges from the computation
// instead of a forced color. Values are diffuse attenuation coefficients in
// m-1 for clear oceanic water (Jerlov type I)
constexpr double WaterAttenuationR = 0.33;
constexpr double WaterAttenuationG = 0.05;
constexpr double WaterAttenuationB = 0.02;

} // namespace AtmospherePhysics

namespace EnvironmentDefaults
{

//-------------------------------------------------------------------------------------------------
// Scene configuration defaults.
//-------------------------------------------------------------------------------------------------

constexpr double AtmosphereAltitude = 100000.0;
constexpr double FogMinimumDistance = 50.0;
constexpr double UnderwaterFogDistance = 1000.0;

constexpr double GlobalAmbientR = 0.05;
constexpr double GlobalAmbientG = 0.05;
constexpr double GlobalAmbientB = 0.15;

constexpr double SkyColorR = 0.2;
constexpr double SkyColorG = 0.6;
constexpr double SkyColorB = 1.0;

constexpr double FogColorR = 0.5;
constexpr double FogColorG = 0.5;
constexpr double FogColorB = 0.5;

constexpr double OzoneAmount = 1.0;

constexpr double Contrast = 1.3;
constexpr double Saturation = 1.0;
constexpr double Brightness = 1.0;

// Sky display exposure. The physical sky outputs small radiance values, this
// exposure maps them to display range, calibrated so the noon zenith matches
// the previous look
constexpr double SkyExposure = 32.0;

} // namespace EnvironmentDefaults

namespace SkyModels
{

//-------------------------------------------------------------------------------------------------
// Sky rendering model identity.
//
// The scene XML selects the sky model by name. Only one model exists for now,
// additional models add their own name and id here. PhysicalId must match the
// SKY_MODEL_PHYSICAL define in the shared GLSL sky include
// (RayleighSky.inc.c), verified by CTestScene::testSkyModelIdMatchesShader.
//-------------------------------------------------------------------------------------------------

constexpr const char* PhysicalName = "Physical";
constexpr int PhysicalId = 0;
constexpr const char* DefaultName = PhysicalName;

} // namespace SkyModels

namespace SkyUniforms
{

//-------------------------------------------------------------------------------------------------
// Shader uniform names shared by the C++ sky models and the GLSL sky code.
//-------------------------------------------------------------------------------------------------

constexpr const char* Model = "u_sky_model";
constexpr const char* SunDirection = "u_sun_direction";
constexpr const char* RayleighBeta = "u_rayleigh_beta";
constexpr const char* RayleighScaleHeight = "u_rayleigh_scale_height";
constexpr const char* OzoneBeta = "u_ozone_beta";
constexpr const char* OzoneAmount = "u_ozone_amount";
constexpr const char* SkyExposure = "u_sky_exposure";
constexpr const char* SkyZenithColor = "u_sky_zenith_color";

} // namespace SkyUniforms
