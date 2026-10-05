#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

vec3 sunLight  = normalize( vec3(  0.35, 0.1,  0.7 ) );
vec3 sunColour = vec3(1.0, 0.75, 0.4);

vec3 cloud_normal = vec3(0.0, 1.0, 0.0);

float layer_altitude[2];
float layer_radius[2];
float layer_coverage[2];

float cloud_x[9];
float cloud_z[9];

float hash(in vec3 p)
{
	return fract(sin(dot(p, vec3(127.1, 311.7, 321.4))) * 43758.5453123);
}

float perlin(in vec3 p)
{
    vec3 i = floor(p);
	vec3 f = fract(p); 
	f *= f * (3. - 2. * f);

    return mix(
		mix(mix(hash(i + vec3(0.,0.,0.)), hash(i + vec3(1.,0.,0.)),f.x),
			mix(hash(i + vec3(0.,1.,0.)), hash(i + vec3(1.,1.,0.)),f.x),
			f.y),
		mix(mix(hash(i + vec3(0.,0.,1.)), hash(i + vec3(1.,0.,1.)),f.x),
			mix(hash(i + vec3(0.,1.,1.)), hash(i + vec3(1.,1.,1.)),f.x),
			f.y),
		f.z) * 2.0;
}

float getCloudTurbulenceAt(vec3 ipos, float scale)
{
	vec3 pos = ipos * scale;

	float f = 0.0;

	for (int i = 0; i < 8; i++)
	{
		pos = (pos.yzx + pos.zyx * vec3(1.0, -1.0, 1.0)) / sqrt(2.0);
		f = f * 2.0 + abs(perlin(pos) - 0.5) * 2.0;
		pos *= 2.0;
	}

	f /= exp2(float(8));
	return f;
}

float getCloudDensityAt(vec3 origin)
{
	float density_factor = 0.4;
	float local_density = 0.0;
	int got_normal = 0;

	for (int l = 0; l < 1; l++)
	{
		if (abs(origin.y - layer_altitude[l]) < (layer_radius[l] * 2.0))
		{
			for (int i = 0; i < 9; i++)
			{
				vec3 position = origin;
				vec3 sphere;

				position.x += time * 5.0;
				
				// Compute the closest sphere's position
				int sx = int((position.x) / (layer_radius[l] * 2.0));
				int sz = int((position.z) / (layer_radius[l] * 2.0));

				float bx = float(sx);
				float bz = float(sz);

				if (bx > 0.0) bx += 0.5; else if (bx < 0.0) bx -= 0.5;
				if (bz > 0.0) bz += 0.5; else if (bz < 0.0) bz -= 0.5;

				sphere.x += cloud_x[i];
				sphere.z += cloud_z[i];

				sphere.x = bx * (layer_radius[l] * 2.0);
				sphere.y = layer_altitude[l];
				sphere.z = bz * (layer_radius[l] * 2.0);

				vec3 sphere_final = sphere; 

				sphere_final.x += perlin(sphere * 0.1) * (layer_radius[l] / 4.0);
				sphere_final.y += perlin(sphere * 0.1) * (layer_radius[l] / 2.0);
				sphere_final.z += perlin(sphere * 0.1) * (layer_radius[l] / 4.0);

				// Is there a sphere here?
				if (perlin((sphere_final * 0.1)) > layer_coverage[l])
				{
					float sphere_radius = (layer_radius[l] * 0.5) + (perlin(sphere * 0.1) * layer_radius[l] * 0.5);
					// float sphere_radius = layer_radius[l];

					// Compute position difference and scale y
					vec3 diff = position - sphere_final; diff.y *= 1.2;

					// compute distance to center
					float dist = length(diff);

					// Are we inside the sphere?
					if (dist < sphere_radius * 0.9)
					{
						if (got_normal == 0)
						{
							got_normal = 1;
							cloud_normal = normalize(position - sphere_final);
						}

						float a = pow(1.0 - (dist / sphere_radius), 3.0) * 2.0;
						float b = pow(getCloudTurbulenceAt(position - sphere_final, 0.001), 3.0) * 1.0;
						float c = (a * b) * density_factor;

						local_density += c;
						//local_density = 1.0;
					}
				}

				if (local_density >= 1.0) break;
			}
		}

		if (local_density >= 1.0) break;
	}

	return local_density;
}

vec4 getCloudsAt(vec3 origin, vec3 direction)
{
	float max_trace_distance = 6000.0;
	int num_steps = 40;
	float step_size = max_trace_distance / float(num_steps);

	if (step_size < 40.0) step_size = 40.0;
	
	float total_density = 0.0;

	vec3 step_distance = direction * step_size;
	vec3 color_light = vec3(0.8, 0.8, 0.8);
	vec3 color_dark = vec3(0.5, 0.5, 0.6);
	vec3 position = origin;

	float accumulated_color = 1.0;

	for (int i = 0; i < 40; i++)
	{
		float local_density = getCloudDensityAt(position);

		total_density += local_density;
		accumulated_color += (cloud_normal.y * local_density) * 0.2;

		position += step_distance;
		if (total_density >= 1.0) break;
	}

	vec4 color;

	color.a = clamp(total_density, 0.0, 1.0) * 0.9;
	color.rgb = mix(color_dark, color_light, clamp(accumulated_color, 0.0, 1.0)); // clamp(color.rgb, 0.6, 0.9);

	return color;
}

#define MAX_ITER 10
#define AURORA_ALT 1000.0

float getAuroraDensityAt(vec3 origin)
{
	if (origin.y < AURORA_ALT) return 0.0;

	vec2 p = origin.xz * 0.001 - vec2(15.0);
	vec2 i = p;
	float c = 10.0;
	float inten = .05;

	for (int n = 0; n < MAX_ITER; n++) 
	{
		float t = time / 20. * (1.0 - (3.0 / float(n+1)));
		i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
		c += 1.0/length(vec2(p.x / (2.*sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
	}

	c /= float(MAX_ITER);
	c = pow(c, 2.0) - 3.1;
	c = c * (clamp(perlin(origin * 0.001), 0.5, 1.0));
	// c = c * clamp(origin.y - AURORA_ALT, 0.0, 1.0);

	return c;
}

vec4 getAurorasAt(vec3 origin, vec3 direction)
{
	float max_trace_distance = 8000.0;
	int num_steps = 80;
	float step_size = max_trace_distance / float(num_steps);
	
	float total_density = 0.0;

	vec3 step_distance = direction * step_size;
	vec3 color_1 = vec3(0.4, 1.0, 0.4);
	vec3 color_2 = vec3(1.0, 0.4, 0.4);
	vec3 position = origin;

	vec3 accumulated_color = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < 80; i++)
	{
		float local_density = getAuroraDensityAt(position);

		total_density += local_density;
		accumulated_color += clamp(mix(color_1, color_2, (position.y - AURORA_ALT) / 2000.0), 0.0, 1.0) * (local_density * 0.1);
		position += step_distance;
	}

	vec4 color;

	vec3 clamped_color = accumulated_color;

	color.a = clamp(total_density, 0.0, 1.0) * 0.5;
	color.rgb = clamped_color;

	return color;
}

vec3 getSkyAt(vec3 origin, vec3 direction)
{
	float sunAmount = max( dot(direction, sunLight), 0.0 );
	float v = pow(1.0-max(direction.y,0.0),6.);
	vec3  sky = mix(vec3(0.0, 0.2, 0.6), vec3(.2, .4, .6), v);
	sky *= smoothstep(-0.3, .0, direction.y);
	sky = sky + sunColour * sunAmount * sunAmount * .25;
	sky = sky + sunColour * min(pow(sunAmount, 800.0)*1.5, .3);
	return clamp(sky, 0.0, 1.0);
}

void main(void)
{
	vec2 xy = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;

	// camera	
	float px = 1200.0;
	float py = 600.0 + 400.0 * cos(0.3 * time);
	float pz = 300.0 * time;

	float tx = 1400.0 + 2000.0 * cos(0.3 * time);
	float ty = 1000.0 + 2000.0 * cos(0.3 * time);
	float tz = 400.0 * time;

	vec3 ro = vec3(0.0, 0.0, 20.0);
	vec3 ta = vec3(-1.0, 5.0, 40.0);
	float roll = 0.0; // 0.4 * sin(0.3 * time);

	// camera tx
	vec3 cw = normalize(ta - ro);
	vec3 cp = vec3(sin(roll), cos(roll),0.0);
	vec3 cu = normalize(cross(cw,cp));
	vec3 cv = normalize(cross(cu,cw));
	vec3 rd = normalize( xy.x * cu + xy.y * cv + 1.75 * cw);

	cloud_x[0] = -1.0;
	cloud_z[0] = -1.0;
	cloud_x[1] =  0.0;
	cloud_z[1] = -1.0;
	cloud_x[2] =  1.0;
	cloud_z[2] = -1.0;
	cloud_x[3] = -1.0;
	cloud_z[3] =  0.0;
	cloud_x[4] =  0.0;
	cloud_z[4] =  0.0;
	cloud_x[5] =  1.0;
	cloud_z[5] =  0.0;
	cloud_x[6] = -1.0;
	cloud_z[6] =  1.0;
	cloud_x[7] =  0.0;
	cloud_z[7] =  1.0;
	cloud_x[8] =  1.0;
	cloud_z[8] =  1.0;

	layer_altitude[0] = 500.0;
	layer_radius[0] = 1000.0;
	layer_coverage[0] = -1.0;

	layer_altitude[1] = 2300.0;
	layer_radius[1] = 1000.0;
	layer_coverage[1] = -1.0;

	vec3 sky = getSkyAt(ro, rd);

	vec4 aurora = getAurorasAt(ro, rd);
	vec3 res = mix(sky, aurora.rgb, aurora.a);

	// vec4 clouds = getCloudsAt(ro, rd);
	// res = mix(res, clouds.rgb, clouds.a);

	gl_FragColor = vec4(res, 1.0);
}
