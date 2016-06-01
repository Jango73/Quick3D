
// Standard vertex shader

#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_gpu_shader5 : enable

#ifdef GL_ES
precision highp int;
precision highp float;
#endif

#define pi 3.1415926535

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
uniform vec3			u_world_up;
uniform float			u_camera_altitude;
uniform float			u_atmosphere_altitude;

uniform vec3			u_global_ambient;

uniform int				u_num_lights;
uniform vec3			u_light_position[8];
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

// Incoming data from vertices

attribute vec3			a_position;
attribute vec2			a_texcoord;
attribute vec3			a_difftext_weight_0_1_2;
attribute vec3			a_difftext_weight_3_4_5;
attribute vec3			a_difftext_weight_6_7_8;
attribute vec3			a_normal;
attribute vec3			a_tangent;
attribute float			a_altitude;

//-------------------------------------------------------------------------------------------------

float distance(vec3 pos)
{
	return abs(sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z));
}

//-------------------------------------------------------------------------------------------------
// Perlin noise

float perlin_hash(in vec3 p)
{
	return fract(sin(dot(p, vec3(127.1, 311.7, 321.4))) * 43758.5453123);
}

float perlin(in vec3 p)
{
    vec3 i = floor(p);
	vec3 f = fract(p); 
	f *= f * (3.0 - 2.0 * f);

    float n = mix(
		mix(mix(perlin_hash(i + vec3(0.0, 0.0, 0.0)), perlin_hash(i + vec3(1.0, 0.0, 0.0)), f.x),
			mix(perlin_hash(i + vec3(0.0, 1.0, 0.0)), perlin_hash(i + vec3(1.0, 1.0, 0.0)), f.x),
			f.y),
		mix(mix(perlin_hash(i + vec3(0.0, 0.0, 1.0)), perlin_hash(i + vec3(1.0, 0.0, 1.0)), f.x),
			mix(perlin_hash(i + vec3(0.0, 1.0, 1.0)), perlin_hash(i + vec3(1.0, 1.0, 1.0)), f.x),
			f.y),
		f.z);

	return clamp((n - 0.49) * 2.05, -1.0, 1.0);
}

float perlin_0_1(in vec3 p)
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

//-------------------------------------------------------------------------------------------------
// Water

float waveHeight(vec3 input_position)
{
	vec3 position_1 = input_position;
	vec3 position_2 = input_position + vec3(2.0, 2.0, 2.0);

	float value_1 = turbulence((position_1 * 0.005) + vec3(u_time, u_time, u_time) * 0.1) * u_wave_amplitude * 4.0;
	float value_2 = turbulence((position_2 * 0.005) - vec3(u_time, u_time, u_time) * 0.1) * u_wave_amplitude * 4.0;

	// float value_3 = turbulence((position_1 * 0.050) + vec3(u_time, u_time, u_time) * 0.5) * u_wave_amplitude * 0.8;
	// float value_4 = turbulence((position_2 * 0.050) - vec3(u_time, u_time, u_time) * 0.5) * u_wave_amplitude * 0.8;

	float alpha_factor = clamp(((a_altitude * -1.0) / 40.0), 0.0, 1.0);
	// float alpha_factor = 1.0;

	// return (value_1 + value_2 + value_3 + value_4) * alpha_factor;
	return value_1 + value_2;
}

//-------------------------------------------------------------------------------------------------
// Lights

vec3 doLighting(vec3 position, vec3 normal, vec3 eye)
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
				vec3 reflection = vec3(0.0, 0.0, 0.0);

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

void main()
{
	vec4 vertex_pos = u_model_matrix * vec4(a_position, 1.0);
	vec4 normal = u_model_matrix * vec4(a_normal, 0.0);
	vec4 tangent = u_model_matrix * vec4(a_tangent, 0.0);
	vec3 binormal = normalize(cross(normal.xyz, tangent.xyz));
	vec4 shadow_coord = u_shadow_projection_matrix * (u_shadow_matrix * vertex_pos);

	// Distance
	float distance = length((u_camera_matrix * vertex_pos).xyz);

	// Compute wave height and normal
	if (bool(u_wave_enable))
	{
		float waveheight = waveHeight(vertex_pos.xyz);
		vertex_pos += (normal * waveheight);
	}

	// Assign varyings
	if (bool(u_rendering_shadows) || u_shaderQuality < 0.10)
	{
		v_color = u_material_diffuse;
	}
	else
	{
		v_color = vec4(doLighting(vertex_pos.xyz, normal.xyz, u_camera_direction.xyz), u_material_diffuse.a);
	}

	v_position = vertex_pos.xyz;
	v_distance = distance;

	if (!bool(u_rendering_shadows))
	{
		v_normal = normal.xyz;
		v_tangent = tangent.xyz;
		v_binormal = binormal.xyz;
		v_texcoord = a_texcoord;
		v_shadow_coord = shadow_coord;
		v_altitude = a_altitude;

		v_difftex_weight_0 = a_difftext_weight_0_1_2.x;
		v_difftex_weight_1 = a_difftext_weight_0_1_2.y;
		v_difftex_weight_2 = a_difftext_weight_0_1_2.z;
		v_difftex_weight_3 = a_difftext_weight_3_4_5.x;
		v_difftex_weight_4 = a_difftext_weight_3_4_5.y;
		v_difftex_weight_5 = a_difftext_weight_3_4_5.z;
		v_difftex_weight_6 = a_difftext_weight_6_7_8.x;
		v_difftex_weight_7 = a_difftext_weight_6_7_8.y;
	}

	// Vertex screen position
	gl_Position = u_camera_projection_matrix * (u_camera_matrix * vertex_pos);
}
