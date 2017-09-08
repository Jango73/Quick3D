#version 430

// Line fragment shader

#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_gpu_shader5 : enable
#extension GL_EXT_geometry_shader4 : enable

#ifdef GL_ES
precision highp int;
precision highp float;
#endif

// Constant incoming data

uniform vec2			u_resolution;
uniform float			u_time;
uniform float			u_deltaTime;
uniform float			u_shaderQuality;
uniform int				u_rendering_shadows;
uniform int				u_normals_only;

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
uniform int     		u_light_is_sun[8];
uniform vec3			u_light_position[8];
uniform vec3			u_light_screen_position[8];
uniform vec3			u_light_direction[8];
uniform vec3			u_light_color[8];
uniform float			u_light_distance_to_camera[8];
uniform float			u_light_distance[8];
uniform float			u_light_spot_angle[8];
uniform float			u_light_occlusion[8];

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
uniform vec3			u_sun_color;

uniform int				u_IR_enable;
uniform float			u_IR_factor;

uniform int				u_inverse_polarity_enable;

// Interpolated values

varying vec3			vo_position;
varying vec3			vo_normal;
varying vec3			vo_tangent;
varying vec3			vo_binormal;
varying vec3			vo_texcoord;
varying vec4			vo_shadow_coord;
varying float			vo_distance;
varying float			vo_altitude;
varying float			vo_difftex_weight_0;
varying float			vo_difftex_weight_1;
varying float			vo_difftex_weight_2;
varying float			vo_difftex_weight_3;
varying float			vo_difftex_weight_4;
varying float			vo_difftex_weight_5;
varying float			vo_difftex_weight_6;
varying float			vo_difftex_weight_7;

//-------------------------------------------------------------------------------------------------

void main()
{
    if (bool(u_rendering_shadows))
    {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);
    }
    else
    {
        // gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        gl_FragColor = u_material_diffuse;
    }
}
