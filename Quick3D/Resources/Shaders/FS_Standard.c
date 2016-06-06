
// Standard fragment shader

#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_gpu_shader5 : enable

#ifdef GL_ES
precision highp int;
precision highp float;
#endif

#define ROOTTHREE		1.73205081
#define uTMK			20.0
#define MAX_STEPS		64
#define TM_MIN			0.05
#define pi				3.1415926535

// Constant incoming data

uniform vec2			u_resolution;
uniform float			u_time;
uniform float			u_deltaTime;
uniform float			u_shaderQuality;
uniform int				u_rendering_shadows;

uniform mat4			u_camera_projection_matrix;
uniform mat4			u_camera_matrix;
uniform mat4			u_shadow_projection_matrix;
uniform mat4			u_shadow_matrix;
uniform mat4			u_model_matrix;
uniform vec3			u_camera_true_position;
uniform vec3			u_camera_position;
uniform vec3			u_camera_direction;
uniform vec3			u_camera_up;
uniform vec3			u_world_origin;
uniform vec3			u_world_up;
uniform float			u_camera_altitude;
uniform float			u_atmosphere_altitude;

uniform vec3			u_global_ambient;

uniform int				u_num_lights;
uniform vec3			u_light_position[8];
uniform vec3			u_light_screen_position[8];
uniform vec3			u_light_direction[8];
uniform vec3			u_light_color[8];
uniform float			u_light_distance[8];
uniform float			u_light_spot_angle[8];

uniform int				u_texture_diffuse_enable;
uniform sampler2D		u_texture_diffuse_0;
uniform sampler2D		u_texture_diffuse_1;
uniform sampler2D		u_texture_diffuse_2;
uniform sampler2D		u_texture_diffuse_3;
uniform sampler2D		u_texture_diffuse_4;
uniform sampler2D		u_texture_diffuse_5;
uniform sampler2D		u_texture_diffuse_6;
uniform sampler2D		u_texture_diffuse_7;
uniform sampler2D		u_shadow_texture;

uniform vec4			u_material_ambient;
uniform vec4			u_material_diffuse;
uniform vec4			u_material_specular;
uniform vec4			u_material_subdermal;
uniform float			u_material_self_illum;
uniform float			u_material_shininess;
uniform float			u_material_reflection;
uniform float			u_material_reflection_steepness;
uniform float			u_material_sss_factor;
uniform float			u_material_sss_radius;

uniform int				u_depth_computing;
uniform int				u_shadow_enable;

uniform int				u_sky_enable;

uniform int				u_wave_enable;
uniform float			u_wave_amplitude;

uniform int				u_fog_enable;
uniform float			u_fog_distance;
uniform vec3			u_fog_color;

uniform int				u_IR_enable;
uniform float			u_IR_factor;

uniform int				u_inverse_polarity_enable;

// Interpolated values

varying vec4			v_color;
varying vec3			v_position;
varying vec3			v_normal;
varying vec3			v_tangent;
varying vec3			v_binormal;
varying vec2			v_texcoord;
varying vec4			v_shadow_coord;
varying float			v_distance;
varying float			v_altitude;
varying float			v_difftex_weight_0;
varying float			v_difftex_weight_1;
varying float			v_difftex_weight_2;
varying float			v_difftex_weight_3;
varying float			v_difftex_weight_4;
varying float			v_difftex_weight_5;
varying float			v_difftex_weight_6;
varying float			v_difftex_weight_7;

// Globals

float	gAtmosphereFactor;
float	gSeaAltitudeFactor_1;
float	gSeaAltitudeFactor_2;
float	gStepSize;
float	gStepFactor;

//-------------------------------------------------------------------------------------------------

float distance(vec3 pos)
{
    return abs(sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z));
}

//-------------------------------------------------------------------------------------------------

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

//-------------------------------------------------------------------------------------------------
// Random

float random(vec2 p)
{
    // We need irrationals for pseudo randomness.
    // Most (all?) known transcendental numbers will (generally) work.
    const vec2 r = vec2(
                23.1406926327792690,	// e^pi (Gelfond's constant)
                2.6651441426902251		// 2^sqrt(2) (Gelfond–Schneider constant)
                );

    return fract(cos(mod(123456789.0, 1e-7 + 256.0 * dot(p,r))));
}

//-------------------------------------------------------------------------------------------------
// Perlin noise

float perlin_hash(vec3 p)
{
    return fract(sin(dot(p, vec3(127.1, 311.7, 321.4))) * 43758.5453123);
}

float perlin(vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);
    f *= f * (3.0 - 2.0 * f);

    float n = mix(
                mix(mix(perlin_hash(i + vec3(0.0, 0.0, 0.0)), perlin_hash(i + vec3(1.0, 0.0, 0.0)),f.x),
                    mix(perlin_hash(i + vec3(0.0, 1.0, 0.0)), perlin_hash(i + vec3(1.0, 1.0, 0.0)),f.x),
                    f.y),
                mix(mix(perlin_hash(i + vec3(0.0, 0.0, 1.0)), perlin_hash(i + vec3(1.0, 0.0, 1.0)),f.x),
                    mix(perlin_hash(i + vec3(0.0, 1.0, 1.0)), perlin_hash(i + vec3(1.0, 1.0, 1.0)),f.x),
                    f.y),
                f.z);

    return clamp((n - 0.49) * 2.05, -1.0, 1.0);
}

float perlin_0_1(vec3 p)
{
    return (perlin(p) + 1.0) / 2.0;
}

//-------------------------------------------------------------------------------------------------
// Turbulence

#define TURBULENCE_OCTAVES 4

float turbulence(vec3 ipos)
{
    vec3 pos = ipos;

    float f = 0.0;

    for (int i = 0; i < TURBULENCE_OCTAVES; i++)
    {
        pos = (pos.yzx + pos.zyx * vec3(1.0, -1.0, 1.0)) / sqrt(2.0);
        f = f * 2.0 + perlin(pos);
        pos *= 1.5;
    }

    f /= pow(2.0, float(TURBULENCE_OCTAVES));
    return f;
}

float turbulence_0_1(vec3 ipos)
{
    vec3 pos = ipos;

    float f = 0.0;

    for (int i = 0; i < TURBULENCE_OCTAVES; i++)
    {
        pos = (pos.yzx + pos.zyx * vec3(1.0, -1.0, 1.0)) / sqrt(2.0);
        f = f * 2.0 + perlin_0_1(pos);
        pos *= 1.5;
    }

    f /= pow(2.0, float(TURBULENCE_OCTAVES));
    return f;
}

float wavelet_pattern(vec3 ipos)
{
    vec3 pos = ipos * 4.0;

    float x = mod(ipos.x, pi * 0.5);
    float y = mod(ipos.y, pi * 0.5);
    float z = mod(ipos.z, pi * 0.5);

    float f = sin(ipos.x) + sin(ipos.y) + sin(ipos.z);

    return f;
}

float movingTurbulence(vec3 ipos, float timeScale)
{
    float time = u_time * timeScale;

    return pow(turbulence_0_1(ipos + vec3(1.0, 1.0, 1.0) * time), 2.0);
    // return wavelet_pattern(ipos + vec3(1.0, 1.0, 1.0) * time);
}

//-------------------------------------------------------------------------------------------------
// Sky and clouds

vec3 cloud_normal = vec3(0.0, 1.0, 0.0);

#define CLOUD_OCTAVES 4

float getCloudTurbulenceAt(vec3 pos)
{
    float f = 0.0;

    for (int i = 0; i < CLOUD_OCTAVES; i++)
    {
        pos = (pos.yzx + pos.zyx * vec3(1.0, -1.0, 1.0)) / sqrt(2.0);
        f = f * 2.0 + abs(perlin(pos) - 0.5) * 2.0;
        pos *= 2.0;
    }

    f /= exp2(float(CLOUD_OCTAVES));
    return f;
}

float getCloudDensityAt(vec3 origin, vec3 position)
{
    float density_factor = 0.4;
    vec3 up = normalize(u_camera_true_position);
    float dAltitude = (up.x * position.x + up.y * position.y + up.z * position.z) + u_camera_altitude;

    float layer_min = 1000.0 - perlin_0_1(position * 0.01) * 300.0;
    float layer_max = 1000.0 + perlin_0_1(position * 0.01) * 300.0;

    if (dAltitude < layer_min) return 0.0;
    if (dAltitude > layer_max) return 0.0;

    float layer_height = layer_max - layer_min;

    float alt_factor_1 = abs((dAltitude - layer_min) / layer_height);
    float alt_factor_2 = abs((layer_max - dAltitude) / layer_height);
    float alt_factor = min(alt_factor_1, alt_factor_2);

    float a = pow(perlin(position * 0.01), 3.0);
    float b = pow(getCloudTurbulenceAt(position * 0.01), 4.0);
    float c = (a + b) * density_factor * alt_factor;

    vec3 normal_position = vec3(0.0, (perlin(position * 0.01) - 0.5) * 2.0, 0.0);
    cloud_normal = normalize(vec3(0.0, 1.0, 0.0) + (origin - normal_position));

    return c;
}

#define CLOUD_STEPS 20

vec4 getCloudsAt(vec3 origin, vec3 direction)
{
    float max_trace_distance = 2000.0;
    float step_size = max_trace_distance / float(CLOUD_STEPS);
    float total_density = 0.0;
    float fog_intensity = clamp(((u_fog_color.r + u_fog_color.g + u_fog_color.b) / 3.0) * 1.5, 0.0, 1.0);

    vec3 step_distance = direction * step_size;
    vec3 color_light = vec3(0.9, 0.9, 0.9) * fog_intensity;
    vec3 color_dark = vec3(0.5, 0.5, 0.6) * fog_intensity;
    vec3 position = origin;

    float accumulated_color = 1.0;

    for (int i = 0; i < CLOUD_STEPS; i++)
    {
        float local_density = getCloudDensityAt(origin, position);

        total_density += local_density;
        accumulated_color += (cloud_normal.y * local_density);

        position += step_distance;
    }

    vec4 color;

    color.a = clamp(total_density, 0.0, 1.0);
    color.rgb = mix(color_dark, color_light, clamp(accumulated_color, 0.0, 1.5));

    return color * gAtmosphereFactor;
}

vec3 stars(vec3 direction)
{
    float n = clamp(perlin(direction * 500.0), 0.95, 1.0);
    float a = pow(n, 80.0);
    return vec3(a);
}

vec3 lightGlows(vec3 origin, vec3 direction, vec2 xy)
{
    vec3 color = vec3(0.0, 0.0, 0.0);

    for (int index = 0; index < u_num_lights; index++)
    {
        vec3 lightDirection = normalize(u_light_position[index] - origin);

        // Large glow
        float amount = max(dot(direction, lightDirection), 0.0);
        color += (u_light_color[index] * amount * amount * 0.25) * gAtmosphereFactor;
        color += (u_light_color[index] * min(pow(amount, 1000.0), 1.0));

        // Ray
        float diffX = abs(xy.x - u_light_screen_position[index].x);
        float mixX = mix(1.0, 0.90, diffX);
        float rayAmountX = clamp(mixX, 0.0, 1.0);
        float diffY = abs(xy.y - u_light_screen_position[index].y);
        float mixY = mix(0.9, 0.00, diffY);
        float rayAmountY = clamp(mixY, 0.0, 1.0);
        float final = pow(rayAmountX * rayAmountY, 10.0);
        color += u_light_color[index] * final;
    }

    return color;
}

vec3 getSkyAt(vec3 origin, vec3 direction, vec2 xy)
{
    vec3 earthDirection = normalize(-u_world_origin - origin);
    vec3 zenithColor = u_fog_color;
    vec3 horizonColor = u_light_color[0];
    vec3 earthGlowColor = vec3(0.5, 0.75, 1.0);
    float earthGlowAmount = max(dot(direction, earthDirection), 0.0);
    float up = pow(1.0 - dot(u_world_up, direction), 6.0);

    vec3 sky = mix(zenithColor, horizonColor, up);

    sky *= smoothstep(-0.3, 0.0, up);
    sky *= gAtmosphereFactor;

    // Earth glow
    if (u_camera_altitude > u_atmosphere_altitude)
    {
        sky += (earthGlowColor * min(pow(earthGlowAmount, 25.0), 1.0));
    }

    // Light glows
    sky += lightGlows(origin, direction, xy);

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

//-------------------------------------------------------------------------------------------------
// Water

float oceanDistanceFieldDetail(vec3 pos, float scale, float timeScale, float amplitude)
{
    float v1 = movingTurbulence(pos * scale, timeScale) * amplitude * 1.0;
    return pos.y + v1;
}

vec3 oceanNormal(vec3 inputPosition, float scale, float timeScale, float amplitude)
{
    vec3 pos = inputPosition;
    vec3 norm;
    vec2 d = vec2(length(pos) * 0.01, 0.0);

    norm.x = oceanDistanceFieldDetail(pos + d.xyy, scale, timeScale, amplitude) - oceanDistanceFieldDetail(pos - d.xyy, scale, timeScale, amplitude);
    norm.y = oceanDistanceFieldDetail(pos + d.yxy, scale, timeScale, amplitude) - oceanDistanceFieldDetail(pos - d.yxy, scale, timeScale, amplitude);
    norm.z = oceanDistanceFieldDetail(pos + d.yyx, scale, timeScale, amplitude) - oceanDistanceFieldDetail(pos - d.yyx, scale, timeScale, amplitude);

    return normalize(norm);
}

/*
//-------------------------------------------------------------------------------------------------
// Volumetric lights

float getTransmittance(vec3 ro, vec3 rd)
{
    vec3 step = rd * gStepSize;
    vec3 pos = ro;

    float tm = 1.0;

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        tm *= exp(-uTMK * gStepSize * perlin(pos));

        pos += step;

        if (tm < TM_MIN ||
            pos.x > 1.0 || pos.x < 0.0 ||
            pos.y > 1.0 || pos.y < 0.0 ||
            pos.z > 1.0 || pos.z < 0.0)
            break;
    }

    return tm;
}

vec4 rayMarchLight(vec3 ro, vec3 rd)
{
    vec3 step = rd * gStepSize;
    vec3 pos = ro;

    vec3 col = vec3(0.0);   // accumulated color
    float tm = 1.0;         // accumulated transmittance

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        // delta transmittance
        float dtm = exp(-uTMK * gStepSize * perlin(pos));
        tm *= dtm;

        // get contribution per light
        for (int k = 0; k < u_num_lights; ++k)
        {
            if (u_light_direction[k] != vec3(0.0, 0.0, 0.0))
            {
                vec3 ld = normalize(u_light_position[k] - pos);
                float ltm = getTransmittance(pos, ld);
                col += (1.0 - dtm) * u_material_diffuse.xyz * u_light_color[k] * tm * ltm;
            }
        }

        pos += step;

        if (tm < TM_MIN ||
            pos.x > 1.0 || pos.x < 0.0 ||
            pos.y > 1.0 || pos.y < 0.0 ||
            pos.z > 1.0 || pos.z < 0.0)
            break;
    }

    float alpha = 1.0 - tm;
    return vec4(col/alpha, alpha);
}

vec4 volumetricLights(vec3 pos)
{
    vec3 color;

    // step_size = root_three / max_steps ; to get through diagonal
    gStepSize = ROOTTHREE / float(MAX_STEPS);
    gStepFactor = 32.0 * gStepSize;

    vec3 cam = (u_camera_matrix * vec4(0.0, 0.0, 0.0, 0.0)).xyz;

    return rayMarchLight(pos, pos - cam);
}
*/

//-------------------------------------------------------------------------------------------------
// Lights

vec3 doLighting(vec3 position, vec3 normal, vec3 eye, vec2 xy)
{
    vec3 color = vec3(0.0, 0.0, 0.0);

    float selfIllumination = u_material_self_illum;

    if (bool(u_IR_enable))
    {
        if (selfIllumination < u_IR_factor) selfIllumination = u_IR_factor;
    }

    for (int index = 0; index < u_num_lights; index++)
    {
        vec3 diff = u_light_position[index] - position;
        float dist = distance(diff);

        if (u_light_distance[index] == 0.0 || dist < u_light_distance[index])
        {
            // Light ray
            vec3 ray = normalize(diff);

            // Compute distance attenuation
            float attenuation = 1.0;

            if (u_light_distance[index] > 0.0)
            {
                attenuation = 1.0 - (dist / u_light_distance[index]);
            }

            // Compute spot attenuation
            if (u_light_direction[index] != vec3(0.0, 0.0, 0.0))
            {
                float spotcutoff = cos(u_light_spot_angle[index]);

                if (dot(-u_light_direction[index], ray) < spotcutoff)
                {
                    attenuation = 0.0;
                }
            }

            // Continue if attenuation is not 0
            if (attenuation > 0.0)
            {
                // Light angle
                float dot_normal_ray = max(dot(normal, ray), 0.0);

                // Reflected ray
                vec3 reflected = normalize(reflect(ray, normal));

                float dot_reflected_eye = dot(reflected, eye);

                // Diffuse light
                vec3 diffuse = u_material_diffuse.xyz * (u_light_color[index] * dot_normal_ray);

                // Specular light
                vec3 specular;
                vec3 subdermal;

                if (u_material_shininess > 0.0)
                {
                    specular = u_material_specular.xyz * pow(max(dot_reflected_eye, 0.0), u_material_shininess);
                }
                else
                {
                    specular = vec3(0.0, 0.0, 0.0);
                }

                // Reflected light
                vec3 reflection;

                if (u_shaderQuality >= 0.75 && u_material_reflection > 0.0)
                {
                    reflection = getSkyAt(position, reflect(eye, normal), xy) * u_material_reflection;

                    if (u_material_reflection_steepness > 0.0)
                    {
                        float dot_normal_eye = dot(normal, eye);
                        float one_minus_dot_normal_eye = 1.0 - abs(dot_normal_eye);

                        reflection = reflection * clamp(pow(one_minus_dot_normal_eye, u_material_reflection_steepness), 0.0, 1.0);
                    }
                }
                else
                {
                    reflection = vec3(0.0, 0.0, 0.0);
                }

                // Subsurface scattering
                if (u_material_sss_factor > 0.0)
                {
                    subdermal = (u_material_subdermal.xyz * (u_light_color[index] * dot_normal_ray)) * u_material_sss_factor;
                }
                else
                {
                    subdermal = vec3(0.0, 0.0, 0.0);
                }

                // Add color components
                color = color + ((diffuse + subdermal + specular) * attenuation) + reflection;
            }
        }
    }

    // Return material ambient plus light color
    return u_global_ambient + u_material_ambient.xyz + u_material_diffuse.xyz * selfIllumination + color;
}

//-------------------------------------------------------------------------------------------------
// Fog

vec4 computeLinearFog(vec4 color)
{
    // Compute fog distance, increase if rendering sky
    float fog_distance = u_fog_distance;
    if (bool(u_sky_enable)) fog_distance *= 4.0;

    // Compute fog factor based on distance and atmosphere
    float fog_factor = clamp(((fog_distance - v_distance) / fog_distance), 0.0, 1.0);
    fog_factor = (1.0 - fog_factor) * gAtmosphereFactor;

    // Compute attenuated sun color
    float average = (0.2125 * u_light_color[0].r) + (0.7154 * u_light_color[0].g) + (0.0721 * u_light_color[0].b);
    vec3 sun_intensity = vec3(average, average, average);
    vec3 sun_light = mix(u_light_color[0], sun_intensity, 0.5);
    // vec3 sun_light = average;

    // Compute color, mix between fog color and sun color
    vec3 fog_color = mix(u_fog_color, sun_light * 0.75, fog_factor);

    // Attenuate if IR
    if (bool(u_IR_enable)) fog_color *= 0.6;

    return vec4(color.xyz * (1.0 - fog_factor) + fog_color * fog_factor, color.a);
}

//-------------------------------------------------------------------------------------------------
// Effects

vec3 postEffects(vec3 rgb, vec2 xy)
{
    // Contrast / saturation / brightness
#define CONTRAST 1.2
#define SATURATION 1.0
#define BRIGHTNESS 1.0
    rgb = mix(vec3(0.5), mix(vec3(dot(vec3(0.2125, 0.7154, 0.0721), rgb * BRIGHTNESS)), rgb * BRIGHTNESS, SATURATION), CONTRAST);

    // Vignette
    rgb *= 0.4 + 0.5 * pow(40.0 * xy.x * xy.y * (1.0 - xy.x) * (1.0 - xy.y), 0.2);

    return rgb;
}

//-------------------------------------------------------------------------------------------------

void main()
{
    if (bool(u_depth_computing))
    {
        float pixel_distance = 1.0;
        float pixel_incidence = 0.0;

        if (v_distance > 0.0 && v_distance < 10000.0)
        {
            // The normalized distance
            pixel_distance = v_distance / 10000.0;

            // The eye-to-vertex vector
            vec3 eye_to_vertex = normalize(v_position - u_camera_position);

            // Angle d'incidence entre vecteur de vidée et vecteur normal du vertex
            pixel_incidence = clamp(dot(v_normal * -1.0, eye_to_vertex), 0.0, 1.0);
        }

        // Red = distance, green = incidence, blue = 0
        gl_FragColor = vec4(pixel_distance, pixel_incidence, 0.0, 1.0);
    }
    else
    {
        if (bool(u_rendering_shadows))
        {
            if (v_distance >= 1.0 && v_distance < 2000.0)
            {
                float d = (v_distance - 1.0) / 1999.0;

                gl_FragColor = vec4(d, d, d, 1.0);
            }
            else
            {
                gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);
            }
        }
        else
        {
            // Input normal
            vec3 normal = v_normal;

            // In case of water, perturbate normal
            if (bool(u_wave_enable))
            {
                if (u_shaderQuality >= 0.75)
                {
                    float large_amplitude = u_wave_amplitude * 3.0;
                    float small_amplitude = large_amplitude * 2.0;

                    mat3 toLocalTangent = inverse(mat3(
                                                      v_tangent.x, v_normal.x, v_binormal.x,
                                                      v_tangent.y, v_normal.y, v_binormal.y,
                                                      v_tangent.z, v_normal.z, v_binormal.z
                                                      ));

                    // Perturbate normal
                    vec3 normal_1 = oceanNormal(v_position, 0.005, 0.1, large_amplitude).xyz;
                    vec3 normal_2 = vec3(0.0, 0.0, 0.0);
                    vec3 normal_3 = vec3(0.0, 0.0, 0.0);

                    if (v_distance < 1000.0)
                    {
                        float factor = (1000.0 - v_distance) / 1000.0;
                        normal_2 = oceanNormal(v_position, 0.3, 0.5, small_amplitude).xyz * factor;
                    }

                    if (v_distance < 500.0)
                    {
                        float factor = (500.0 - v_distance) / 500.0;
                        normal_3 = oceanNormal(v_position * -1.0, 1.0, 2.0, small_amplitude).xyz * factor;
                    }

                    normal = normalize(toLocalTangent * (normal_1 + normal_2 + normal_3));
                    normal = (u_model_matrix * vec4(normal, 0.0)).xyz;
                }
            }

            // The eye-to-vertex vector
            vec3 eye_to_vertex = normalize(v_position - u_camera_position);

            // Screen coordinates
            vec2 xy = gl_FragCoord.xy / u_resolution.xy;

            // Input color
            vec4 color = vec4(doLighting(v_position, normal, eye_to_vertex, xy), 1.0);

            // Atmosphere factor
            gAtmosphereFactor = clamp((u_atmosphere_altitude - u_camera_altitude) / u_atmosphere_altitude, 0.0, 1.0);

            // Altitude factors for sea
            gSeaAltitudeFactor_1 = clamp((v_altitude * -1.0) / 2.0, 0.0, 1.0);
            gSeaAltitudeFactor_2 = clamp((v_altitude * -1.0) / 10.0, 0.0, 1.0);

            if (bool(u_shadow_enable) && u_num_lights > 0)
            {
                vec3 sc = v_shadow_coord.xyz;

                sc /= v_shadow_coord.w;
                sc += 1.0;
                sc *= 0.5;

                // Get shadow

                if (
                        sc.x > 0.0 && sc.x < 1.0 &&
                        sc.y > 0.0 && sc.y < 1.0 &&
                        sc.z > 0.0 && sc.z < 1.0
                        )
                {
                    float shadow_depth = texture2D(u_shadow_texture, sc.xy).r;

                    if (shadow_depth + 0.005 < sc.z)
                    {
                        color *= vec4(0.25, 0.25, 0.25, 1.0);
                    }
                }
            }

            // vec3 normal_eye_space = (u_camera_matrix * normal) - (u_camera_matrix * vec3(0.0, 0.0, 0.0));
            // vec3 tangent_eye_space = (u_camera_matrix * v_tangent) - (u_camera_matrix * vec3(0.0, 0.0, 0.0));

            // Apply texture if asked
            if (bool(u_texture_diffuse_enable) && u_shaderQuality >= 0.20)
            {
                vec3 texture_color = vec3(0.0, 0.0, 0.0);

                texture_color = mix(texture_color, texture2D(u_texture_diffuse_0, v_texcoord).xyz, v_difftex_weight_0);
                texture_color = mix(texture_color, texture2D(u_texture_diffuse_1, v_texcoord).xyz, v_difftex_weight_1);
                texture_color = mix(texture_color, texture2D(u_texture_diffuse_2, v_texcoord).xyz, v_difftex_weight_2);
                texture_color = mix(texture_color, texture2D(u_texture_diffuse_3, v_texcoord).xyz, v_difftex_weight_3);
                texture_color = mix(texture_color, texture2D(u_texture_diffuse_4, v_texcoord).xyz, v_difftex_weight_4);
                texture_color = mix(texture_color, texture2D(u_texture_diffuse_5, v_texcoord).xyz, v_difftex_weight_5);
                texture_color = mix(texture_color, texture2D(u_texture_diffuse_6, v_texcoord).xyz, v_difftex_weight_6);
                texture_color = mix(texture_color, texture2D(u_texture_diffuse_7, v_texcoord).xyz, v_difftex_weight_7);

                color *= vec4(texture_color, color.a);
            }

            // Apply sky if asked
            if (bool(u_sky_enable))
            {
                color = vec4(getSkyAt(u_camera_position, eye_to_vertex, xy), color.a);
            }

            // Apply water if asked
            if (bool(u_wave_enable))
            {
                if (u_shaderQuality < 0.25)
                {
                }
                else if (u_shaderQuality < 0.75)
                {
                    float large_amplitude = 2.0;
                    float small_amplitude = large_amplitude * 2.0;

                    float v1 = movingTurbulence(v_position * 0.005, 0.1) * large_amplitude;
                    float v2 = movingTurbulence(v_position * 0.3, 0.5) * large_amplitude;
                    float v3 = 0.0;

                    if (v_distance < 500.0)
                    {
                        v3 = movingTurbulence(v_position * 2.0, 0.7) * small_amplitude;
                    }

                    float turb = clamp((v1 + v2 + v3) * 0.5, 1.0, 100.0);

                    color = color * vec4(turb, turb, turb, 1.0);

                    // Compute alpha
                    color.a = gSeaAltitudeFactor_1;
                }
                else
                {

                    // Compute lighting
                    // color = color * 0.5 + vec4(doLighting(v_position, normal, eye_to_vertex), color.a) * 0.5;

                    // Compute alpha
                    color.a = gSeaAltitudeFactor_1;
                }
            }

            // Apply atmospheric light - does not work yet
            // color = color + volumetricLights(v_position);

            // Apply fog
            if (bool(u_fog_enable))
            {
                color = computeLinearFog(color);
            }

            // Apply IR if asked
            if (bool(u_IR_enable))
            {
                float average = 0.2125 * color.r + 0.7154 * color.g + 0.0721 * color.b;
                vec2 seed = vec2(sin(gl_FragCoord.x + u_time), cos(gl_FragCoord.y + u_time));
                float value = average + random(seed) * 0.01;
                color = vec4(value, value, value, color.a);
            }

            // Apply polarity if asked
            if (bool(u_inverse_polarity_enable))
            {
                color.r = 1.0 - color.r;
                color.g = 1.0 - color.g;
                color.b = 1.0 - color.b;
            }

            // Apply effects
            color = vec4(postEffects(color.rgb, xy), color.a);

            // Final color
            gl_FragColor = color;
        }
    }
}
