// Shared physical sky, included by glsl1/glsl4 FS_Standard.c through
// CRessourcesManager::getShaderByFilePathName() #include expansion.
// Single source of truth for the GPU sky: edit here, both shader variants
// follow. Kasten-Young coefficients mirror AtmospherePhysics in
// CEnvironmentDefaults.h, SKY_MODEL_PHYSICAL mirrors SkyModels::PhysicalId,
// keep them in sync when editing either side.

//-------------------------------------------------------------------------------------------------
// Sky uniforms and constants

uniform int             u_sky_model;
uniform vec3            u_sun_direction;
uniform vec3            u_rayleigh_beta;
uniform float           u_rayleigh_scale_height;
uniform vec3            u_ozone_beta;
uniform float           u_ozone_amount;
uniform float           u_sky_exposure;

#define SKY_MODEL_PHYSICAL        0

// Kasten and Young (1989) relative airmass coefficients
#define KASTEN_YOUNG_A            0.50572
#define KASTEN_YOUNG_B            6.07995
#define KASTEN_YOUNG_C            1.6364

// Star field density: direction hash scale, visibility threshold, disc sharpness
#define STAR_FIELD_SCALE          500.0
#define STAR_VISIBILITY_THRESHOLD 0.95
#define STAR_SHARPNESS            80.0

// Sky dome shaping: horizon falloff, horizon fade band, earth glow tightness
#define SKY_UP_FALLOFF_EXPONENT   6.0
#define SKY_HORIZON_FADE_NEAR     -0.3
#define EARTH_GLOW_EXPONENT       25.0

//-------------------------------------------------------------------------------------------------
// Sky

// Rayleigh single scattering with ozone absorption along the view direction,
// same model as the sun tint. The scattering share weighs in-scattered light
// by the Rayleigh part of the extinction, absorbed light does not scatter.
vec3 rayleighSkyAt(vec3 direction)
{
    vec3 viewDirection = normalize(direction);

    float sinView = max(dot(viewDirection, u_world_up), 0.0);
    float viewElevation = degrees(asin(min(sinView, 1.0)));
    float viewAirmass = 1.0 / (sinView + KASTEN_YOUNG_A * pow(viewElevation + KASTEN_YOUNG_B, -KASTEN_YOUNG_C));
    float viewPath = u_rayleigh_scale_height * viewAirmass;

    float mu = dot(viewDirection, u_sun_direction);
    float phase = 3.0 / (16.0 * PI) * (1.0 + mu * mu);

    vec3 extinction = u_rayleigh_beta + u_ozone_amount * u_ozone_beta;
    vec3 scatteringShare = u_rayleigh_beta / extinction;

    return u_sky_exposure * u_sun_color * phase * scatteringShare * (1.0 - exp(-extinction * viewPath));
}

vec3 stars(vec3 direction)
{
    float n = clamp(perlin(direction * STAR_FIELD_SCALE), STAR_VISIBILITY_THRESHOLD, 1.0);
    float a = pow(n, STAR_SHARPNESS);
    // Day sky hides stars only inside the atmosphere, in space the sky stays
    // black at noon so stars remain visible
    return vec3(a) * (1.0 - u_sun_intensity * gAtmosphereFactor);
}

vec3 standardSkyAt(vec3 origin, vec3 direction, vec2 xy)
{
    vec3 earthDirection = normalize(-u_world_origin - origin);
    vec3 earthGlowColor = vec3(0.5, 0.75, 1.0);
    float earthGlowAmount = max(dot(direction, earthDirection), 0.0);
    float up = pow(1.0 - dot(u_world_up, direction), SKY_UP_FALLOFF_EXPONENT);

    vec3 sky;

    if (u_sky_model == SKY_MODEL_PHYSICAL)
    {
        sky = rayleighSkyAt(direction);
    }
    else
    {
        // Defensive fallback, scene files always select a known model
        sky = mix(u_sky_zenith_color, u_sun_color, up);
    }

    sky *= smoothstep(SKY_HORIZON_FADE_NEAR, 0.0, up);
    sky *= gAtmosphereFactor;

    // Earth glow
    if (u_camera_altitude > u_atmosphere_altitude)
    {
        sky += (earthGlowColor * min(pow(earthGlowAmount, EARTH_GLOW_EXPONENT), 1.0));
    }

    // Highlight rolloff on the scattered sky only, stars are added after so
    // the tonemap never dims them. Pixels below 1.0 are untouched, brighter
    // pixels are scaled onto the gamut surface: ratios are preserved exactly,
    // where a hard clamp would clip bright orange to white
    sky /= max(max(sky.r, max(sky.g, sky.b)), 1.0);

    // Stars
    sky += stars(direction);

    // Clamping
    sky = clamp(sky, 0.0, 1.0);

    if (u_shaderQuality < 0.85)
    {
        return sky;
    }

    // Clouds
    vec4 clouds = getCloudsAt(origin, direction);
    return mix(sky, clouds.rgb, clouds.a);
}
