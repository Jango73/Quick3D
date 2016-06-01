#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

vec3 iResolution = vec3(resolution.x,resolution.y,100.);
vec4 iMouse = vec4(mouse.x,mouse.y,5.,5.);
float iGlobalTime = time;
uniform sampler2D iChannel0;

// by @301z

float rand(vec2 n) { 
	return fract(cos(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

// Genera ruido en funci�n de las coordenadas del pixel
float noise(vec2 n) {
	const vec2 d = vec2(0.0, 1.0);
	vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

// Fractional Brownian Amplitude. Suma varias "capas" de ruido.
float fbm(vec2 n) {
	float total = 0.0, amplitude = 1.0;
	for (int i = 0; i < 4; i++) {
		total += noise(n) * amplitude;
		n += n;
		amplitude *= 0.5;
	}
	return total;
}

void main() {
	// Colores
	const vec3 c1 = vec3(0.0, 0.0, 0.1); // Rojo oscuro.
	const vec3 c2 = vec3(0.7, 0.0, 0.0); // Rojo claro.
	const vec3 c3 = vec3(0.2, 0.0, 0.0); // Rojo oscuro.
	const vec3 c4 = vec3(1.0, 0.9, 0.0); // Amarillo.
	const vec3 c5 = vec3(0.1); // Gris oscuro.
	const vec3 c6 = vec3(0.9); // Gris claro.
	
	vec2 p = gl_FragCoord.xy * 8.0 / iResolution.xx; // Desfasa las coordenadas para que haya m�s cambio de un resultado a los colindantes.
	float q = fbm(p - iGlobalTime * 0.1); // Ruido con offset para el movimiento.
	vec2 r = vec2(fbm(p + q + iGlobalTime * 0.7 - p.x - p.y), fbm(p + q - iGlobalTime * 0.4));
	vec3 c = mix(c1, c2, fbm(p + r)) + mix(c3, c4, r.x) - mix(c5, c6, r.y);
	gl_FragColor = vec4(c * 
			    cos(1.57 * gl_FragCoord.y / iResolution.y), // Gradiente m�s ocuro arriba.
			    1.0);
}
