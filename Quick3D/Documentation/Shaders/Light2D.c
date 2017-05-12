varying highp vec2 qt_TexCoord0;
uniform lowp float qt_Opacity;

uniform sampler2D source;
uniform sampler2D bump;
uniform sampler2D environment;

uniform highp vec3 lightPosition;
uniform highp vec3 lightDirection;
uniform highp float lightIntensity;
uniform highp float lightDistance;
uniform highp float lightOuterAngle;
uniform highp float lightInnerAngle;
uniform highp vec4 lightColor;
uniform highp float lightFlareIntensity;
uniform highp float lightAreaRadius;
uniform lowp int lightAreaSampleCount;
uniform highp vec4 materialAmbientColor;
uniform highp vec4 materialDiffuseColor;
uniform highp vec4 materialSpecularColor;
uniform highp float materialSpecularShininess;
uniform highp float materialSpecularIntensity;
uniform highp float materialFresnelIOR;
uniform highp float bumpHeight;
uniform lowp int lightShadowSampleCount;
uniform highp float fogAmount;
uniform highp float fogSize;
uniform highp float pixelDistanceX;
uniform highp float pixelDistanceY;

uniform highp float xRatio;
uniform highp float yRatio;

//-------------------------------------------------------------------------------------------------
// Constants

#define TURBULENCE_OCTAVES 4
#define FOG_STEPS 20
#define PI 3.14159265358979323846264

// If defined, will use noise to compute soft shadows
#define AREA_NOISE

//-------------------------------------------------------------------------------------------------
// Math functions

float colorLuminosity(vec4 color)
{
    return (color.r + color.g + color.b) / 3.0;
}

// Brings all values below a threshold smoothly to that threshold
float almostIdentity( float x, float m, float n )
{
    if (x > m) return x;

    float a = 2.0 * n - m;
    float b = 2.0 * m - 3.0 * n;
    float t = x / m;

    return (a * t + b) * t * t + n;
}

// Impulse
float impulse(float k, float x)
{
    float h = k * x;
    return h * exp(1.0 - h);
}

// Cubic pulse
float cubicPulse(float c, float w, float x)
{
    x = abs(x - c);
    if (x > w) return 0.0;
    x /= w;
    return 1.0 - x * x * (3.0 - 2.0 * x);
}

// Exponential step
float expStep(float x, float k, float n)
{
    return exp(-k * pow(x, n));
}

// Parabola
float parabola(float x, float k)
{
    return pow(4.0 * x * (1.0 - x), k);
}

vec4 multQuat(vec4 q1, vec4 q2)
{
    return vec4(
        q1.w * q2.x + q1.x * q2.w + q1.z * q2.y - q1.y * q2.z,
        q1.w * q2.y + q1.y * q2.w + q1.x * q2.z - q1.z * q2.x,
        q1.w * q2.z + q1.z * q2.w + q1.y * q2.x - q1.x * q2.y,
        q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
    );
}

vec3 rotateVector(vec4 quat, vec3 vec)
{
    vec4 qv = multQuat(quat, vec4(vec, 0.0));
    return multQuat(qv, vec4(-quat.x, -quat.y, -quat.z, quat.w)).xyz;
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float fresnel(vec3 eye, vec3 normal, float ior)
{
    float cosi = clamp(dot(eye, normal), -1, 1);
    float etai = 1.0;
    float etat = ior;
    float kr = 0.0;

    if (cosi > 0.0)
    {
        float temp = etai;
        etai = etat;
        etat = temp;
    }

    // Compute sini using Snell's law
    float sint = etai / etat * sqrt(max(0.0, 1.0 - cosi * cosi));

    // Total internal reflection
    if (sint >= 1.0)
    {
        kr = 1.0;
    }
    else
    {
        float cost = sqrt(max(0.0, 1.0 - sint * sint));
        cosi = abs(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2.0;
    }

    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;

    return kr;
}

//-------------------------------------------------------------------------------------------------
// Perlin noise

float perlin_hash(vec3 p)
{
    return fract(sin(dot(p, vec3(127.1, 311.7, 321.4))) * 43758.5453123);
}

// Computes a perlin noise value for p
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

// Computes a perlin noise value between 0 and 1 for p
float perlin_0_1(vec3 p)
{
    return (perlin(p) + 1.0) / 2.0;
}

// Computes a turbulence noise value for ipos
float turbulence(vec3 ipos)
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

//-------------------------------------------------------------------------------------------------

float getHeight(vec2 position)
{
    vec4 color = texture2D(bump, position);

    return ((color.r + color.g + color.b) / 3.0) * bumpHeight;
}

//-------------------------------------------------------------------------------------------------
// Fog

float rayMarchFog(vec3 start, vec3 finish)
{
    vec3 ray = finish - start;
    vec3 rayDirection = normalize(ray);
    float rayLength = distance(start, finish);
    vec3 position = start;
    float stepSize = rayLength / float(FOG_STEPS);
    float totalDensity = 0.0;
    vec3 stepVector = rayDirection * stepSize;

    for (int i = 0; i < FOG_STEPS; i++)
    {
        totalDensity += pow(turbulence(position / fogSize), 3.0) * 0.3;
        position += stepVector;
    }

    return 1.0 - clamp(totalDensity, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------
// Shadows

float rayMarchShadows(vec3 start, vec3 finish)
{
    float finalAttenuation = 0.0;
    int areaIndex, rayIndex;

    int actualAreaSampleCount = lightAreaSampleCount;
    if (actualAreaSampleCount < 1) actualAreaSampleCount = 1;

    for (areaIndex = 0; areaIndex < actualAreaSampleCount; areaIndex++)
    {
        // Compute position in the light

#ifdef AREA_NOISE
        vec2 noiseVec1 = vec2(start.x * 100.0 + float(areaIndex), start.y * 100.0 - float(areaIndex));
        vec2 noiseVec2 = vec2(start.y * 100.0 + float(areaIndex), start.x * 100.0 - float(areaIndex));
        float offsetX = (1.0 - (rand(noiseVec1) * 2.0)) * (lightAreaRadius * 0.5);
        float offsetY = (1.0 - (rand(noiseVec2) * 2.0)) * (lightAreaRadius * 0.5);
        vec3 areaPosition = finish + vec3(offsetX, offsetY, 0.0);
#else
        vec4 quat = vec4(0.0, 0.0, 1.0, float(areaIndex) / float(actualAreaSampleCount));
        vec3 areaPosition = finish + rotateVector(quat, vec3(lightAreaRadius, 0.0, 0.0));
#endif

        vec3 ray = areaPosition - start;
        vec3 rayDirection = normalize(ray);
        float rayLength = distance(start, areaPosition);
        float rayStep = rayLength / float(lightShadowSampleCount);
        vec3 position = start;
        float localAttenuation = 1.0;

        for (rayIndex = 0; rayIndex < lightShadowSampleCount; rayIndex++)
        {
            float obstructionZ = getHeight(position.xy);

            if (position.z < obstructionZ)
            {
                // Here we are in shadow
                localAttenuation = 0.0;
                break;
            }

            position = position + rayDirection * rayStep;
        }

        finalAttenuation += localAttenuation;
    }

    finalAttenuation /= float(actualAreaSampleCount);

    if (finalAttenuation > 0.0)
    {
        if (fogAmount > 0.0)
        {
            finalAttenuation *= mix(1.0, rayMarchFog(start, finish), fogAmount);
        }
    }

    return finalAttenuation;
}

//-------------------------------------------------------------------------------------------------
// Lighting

vec4 lightUp(vec3 position, vec3 normal, vec3 eye)
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);

    //-----------------------------------------------
    // Compute bump normal

    float me = position.z;
    float n = getHeight(position.xy + vec2(0.0, -pixelDistanceY)) * 20.0;
    float s = getHeight(position.xy + vec2(0.0,  pixelDistanceY)) * 20.0;
    float e = getHeight(position.xy + vec2( pixelDistanceX, 0.0)) * 20.0;
    float w = getHeight(position.xy + vec2(-pixelDistanceX, 0.0)) * 20.0;

    // find perpendicular vector to normal
    vec3 temp = normal;
    if (normal.x == 1) temp.y += 0.5; else temp.x += 0.5;

    // form a basis with normal being one of the axes
    vec3 perp1 = normalize(cross(normal, temp));
    vec3 perp2 = normalize(cross(normal, perp1));

    // use the basis to move the normal in its own space by the offset
    vec3 normalOffset = -1.0 * (((n - me) - (s - me)) * perp1 + ((e - me) - (w - me)) * perp2);
    normal = normalize(normal - normalOffset);

    // Uncomment to visualize normals
    // return vec4((normal.xyz + 1.0) / 2.0, 0.0);

    //-----------------------------------------------
    // Comput fresnel factor

    float fresnelFactor = fresnel(eye, normal, materialFresnelIOR);

    // Uncomment to see fresnel effect
    // return vec4(fresnelFactor, fresnelFactor, fresnelFactor, 1.0);

    //-----------------------------------------------
    // Iterate through all lights

    // for (int index = 0; index < u_num_lights; index++)
    {
        float dist = distance(position, lightPosition);

        if (lightDistance == 0.0 || dist < lightDistance)
        {
            // Light ray
            vec3 ray = normalize(lightPosition - position);

            //-----------------------------------------------
            // Compute distance attenuation

            float attenuation = 1.0;

            if (lightDistance > 0.0)
            {
                attenuation = clamp(1.0 - (dist / lightDistance), 0.0, 1.0);
            }

            //-----------------------------------------------
            // Compute spot attenuation

            if (lightDirection != vec3(0.0, 0.0, 0.0) && lightOuterAngle != 0.0)
            {
                float spotOuterCutoff = cos(lightOuterAngle);
                // float spotInnerCutoff = cos(lightInnerAngle);
                float spotEffect = (dot(normalize(lightDirection), -ray));
                // float smoothSpot = (lightOuterAngle / 3.14926535 - lightInnerAngle / 3.14926535);
                float smoothSpot = 0.05;

                if (spotEffect < spotOuterCutoff)
                {
                    spotEffect = smoothstep(spotOuterCutoff - smoothSpot, spotOuterCutoff, spotEffect);
                }
                else
                {
                    spotEffect = 1.0;
                }

                attenuation *= spotEffect;
            }

            //-----------------------------------------------
            // Continue if attenuation is not 0

            if (attenuation > 0.0)
            {
                // Get shadows
                attenuation *= rayMarchShadows(position, lightPosition);

                // Light angle
                float dotNormalRay = max(dot(normal, ray), 0.0);

                // Reflected ray
                vec3 reflected = normalize(reflect(ray, normal));

                float dotReflectedEye = dot(reflected, eye);

                // Diffuse light
                vec3 diffuse = materialDiffuseColor.rgb * (lightColor.rgb * dotNormalRay) * lightIntensity;

                // Specular light
                specular += materialSpecularColor * pow(max(dotReflectedEye, 0.0), materialSpecularShininess) * materialSpecularIntensity;

                // Add color components
                color.rgb += diffuse * attenuation;
            }
        }
    }

    //-----------------------------------------------
    // Compute reflection

    vec3 eyeReflected = normalize(reflect(eye, normal));
    float yaw = 0.5 - atan(eyeReflected.z, - eyeReflected.x) / (2.0 * PI);
    float pitch = 0.5 - atan(eyeReflected.y, length(eyeReflected.xz)) / PI;
    vec2 environmentUV = vec2(yaw, pitch);
    vec4 environmentColor = texture2D(environment, environmentUV) + specular;

    // Return material ambient plus light color

    return vec4(mix(materialAmbientColor.rgb + color.rgb, environmentColor.rgb, fresnelFactor), 1.0);
}

//-------------------------------------------------------------------------------------------------
// Lens effects

float disc(vec3 discPosition, float discRadius, vec3 position)
{
    float dist = distance(position.xy, discPosition.xy);
    return dist > discRadius ? 0.0 : (discRadius - dist) / discRadius;
}

vec3 flareDisc(vec3 light, float where)
{
    vec3 center = vec3(0.5, 0.5, 0.0);
    vec3 ray = light - center;
    return center - ray * where;
}

vec3 lightRays(vec3 origin, vec3 direction, vec3 pixel)
{
    float final = 0.0;

    if (lightPosition.x >= 0.0 && lightPosition.x <= 1.0 && lightPosition.y >= 0.0 && lightPosition.y <= 1.0)
    {
        float diffX = abs(pixel.x - lightPosition.x) * 0.25;
        float rayAmountX = 1.0 - clamp(diffX, 0.0, 1.0);
        float diffY = abs(pixel.y - lightPosition.y) * 1.0;
        float rayAmountY = 1.0 - clamp(diffY, 0.0, 1.0);
        final = pow(rayAmountX * rayAmountY, 20.0);
    }

    return lightColor * final * lightIntensity * lightFlareIntensity;
}

float flareType1(float input)
{
    return clamp(impulse(8.0, input), 0.0, 1.0);
}

float flareType2(float input)
{
    return clamp(cubicPulse(0.5, 0.5, input), 0.0, 1.0);
}

vec3 lightFlares(vec3 origin, vec3 direction, vec3 pixel)
{
    vec3 final = vec3(0.0, 0.0, 0.0);

    if (lightPosition.x >= 0.0 && lightPosition.x <= 1.0 && lightPosition.y >= 0.0 && lightPosition.y <= 1.0)
    {
        final += clamp(flareType1(disc(flareDisc(vec3(lightPosition.xy, 0.0), 0.2), 0.06, pixel)), 0.0, 1.0) * vec3(1.0, 0.8, 0.8);
        final += clamp(flareType1(disc(flareDisc(vec3(lightPosition.xy, 0.0), 0.3), 0.03, pixel)), 0.0, 1.0) * vec3(0.8, 0.8, 1.0);
        final += clamp(flareType2(disc(flareDisc(vec3(lightPosition.xy, 0.0), 0.4), 0.03, pixel)), 0.0, 1.0) * vec3(0.8, 0.8, 1.0);
        final += clamp(flareType2(disc(flareDisc(vec3(lightPosition.xy, 0.0), 0.7), 0.10, pixel)), 0.0, 1.0) * vec3(1.0, 0.8, 0.8);
    }

    return lightColor * final * lightIntensity * lightFlareIntensity * 0.5;
}

//-------------------------------------------------------------------------------------------------
// Main

void main()
{
    vec3 down = vec3(0.0, 0.0, -1.0);
    // vec3 up = normalize(vec3(0.8, 0.0, 0.2));
    vec3 up = vec3(0.0, 0.0, 1.0);
    vec3 pixel = vec3(qt_TexCoord0.x, qt_TexCoord0.y, getHeight(qt_TexCoord0));
    vec3 eyePosition = vec3(0.5, 0.5, 2.0);
    // vec3 eyeDirection = down;
    vec3 eyeDirection = normalize(pixel - eyePosition);
    vec4 color = texture2D(source, pixel.xy);
    vec4 light = lightUp(pixel, up, eyeDirection);
    vec4 rays = vec4(lightRays(eyePosition, eyeDirection, pixel), 1.0);
    vec4 flares = vec4(lightFlares(eyePosition, eyeDirection, pixel), 1.0);
    float flareLuminosity = colorLuminosity(flares);
    vec4 finalColor = vec4(color.r * light.r, color.g * light.g, color.b * light.b, 1.0) + flares;
    finalColor = mix(finalColor, flares, flareLuminosity);
    gl_FragColor = vec4(finalColor.rgb, finalColor.a * qt_Opacity);
}
