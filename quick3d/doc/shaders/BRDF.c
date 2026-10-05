varying highp vec2 qt_TexCoord0;
uniform lowp float qt_Opacity;

uniform sampler2D source;
uniform sampler2D bumpMap;
uniform sampler2D environmentMap;

uniform highp vec3 lightPosition;
uniform highp vec3 lightDirection;
uniform highp float lightIntensity;
uniform highp float lightDistance;
uniform highp float lightOuterAngle;
uniform highp float lightInnerAngle;
uniform highp vec4 lightColor;
uniform highp float lightFlareIntensity;
uniform lowp int lightShadowSampleCount;
uniform highp float lightAreaRadius;
uniform lowp int lightAreaSampleCount;
uniform lowp int lightIsDirectional;

uniform lowp int environmentSampleCount;

uniform highp vec4 materialAmbientColor;
uniform highp vec4 materialGlossyColor;

uniform highp float bumpAltitude;
uniform highp float bumpHeight;
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
#define _2PI (PI * 2.0)

// If defined, will use noise to compute soft shadows
#define AREA_NOISE

#define PASS_LIGHT  0
#define PASS_ENV    1

vec4 materialDiffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
vec3 down = vec3(0.0, 0.0, -1.0);
vec3 up = vec3(0.0, 0.0, 1.0);
vec3 right = vec3(1.0, 0.0, 0.0);
vec3 rear = vec3(0.0, -1.0, 0.0);

vec3 position;
vec3 eyePosition;
vec3 eyeDirection;
vec3 surfaceNormal;
vec3 surfaceNormalTangent;
vec3 surfaceNormalBitangent;
vec3 lightRay;
float facing;
float facingInverse;
int passType;

//-------------------------------------------------------------------------------------------------
// Math functions

float sqr(float x)
{
    return x * x;
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

vec4 quaternionFromAxisAndAngle(float x, float y, float z, float angle)
{
     vec4 result = vec4(0.0, 0.0, 0.0, 0.0);
     //x, y, and z form a normalized vector which is now the axis of rotation.
     result.w  = cos(angle / 2.0);
     result.x = x * sin(angle / 2.0);
     result.y = y * sin(angle / 2.0);
     result.z = z * sin(angle / 2.0);
     return result;
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

vec3 rotateVectorFast(vec4 quat, vec3 position)
{
    return position + 2.0 * cross(cross(position, quat.xyz) + quat.w * position, quat.xyz);
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 mapDirectionVectorToEqui(vec3 position)
{
    vec2 result;

    result.x = (atan2(position.z, position.x) / _2PI) + 0.25;
    result.y = acos(position.y) / PI;

    result.x = mod(1.0 - result.x, 1.0);
    result.y = mod(1.0 - result.y, 1.0);

    return result;
}

float flatDisc(vec3 discPosition, float discRadius, vec3 position)
{
    float dist = distance(position, discPosition);
    return dist > discRadius ? 0.0 : 1.0;
}

float disc(vec3 discPosition, float discRadius, vec3 position)
{
    float dist = distance(position.xy, discPosition.xy);
    return dist > discRadius ? 0.0 : (discRadius - dist) / discRadius;
}

//-------------------------------------------------------------------------------------------------
// Color functions

float colorLuminosity(vec4 color)
{
    return dot(color.rgb, vec3(0.299, 0.587, 0.114));
}

vec4 contrastSaturationBrightness(vec4 color, float contrast, float saturation, float brightness)
{
    vec3 newColor = mix(vec3(0.5), mix(vec3(dot(vec3(0.2125, 0.7154, 0.0721), color.rgb * brightness)), color.rgb * brightness, saturation), contrast);

    return vec4(newColor.rgb, color.a);
}

//-------------------------------------------------------------------------------------------------
// BRDFs (Bidirectional reflectance distribution function)

float beckmannDistribution(float x, float roughness)
{
    float NdotH = max(x, 0.0001);
    float cos2Alpha = NdotH * NdotH;
    float tan2Alpha = (cos2Alpha - 1.0) / cos2Alpha;
    float roughness2 = roughness * roughness;
    float denom = PI * roughness2 * cos2Alpha * cos2Alpha;
    return exp(tan2Alpha / roughness2) / denom;
}

float phongDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal)
{
    if (passType == PASS_ENV)
    {
        return 0.0;
    }

    return max(dot(surfaceNormal, -lightDirection), 0.0);
}

float orenNayarDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, float albedo)
{
    if (passType == PASS_ENV)
    {
        return 0.0;
    }

    lightDirection = -lightDirection;

    float LdotV = dot(lightDirection, viewDirection);
    float NdotL = dot(lightDirection, surfaceNormal);
    float NdotV = dot(surfaceNormal, viewDirection);

    float s = LdotV - NdotL * NdotV;
    float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

    float sigma2 = roughness * roughness;
    float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
    float B = 0.45 * sigma2 / (sigma2 + 0.09);

    return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}

float beckmannDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness)
{
    if (passType == PASS_ENV)
    {
        return 0.0;
    }

    // Limit roughness to 0.001 - 0.7
    roughness = clamp(roughness, 0.001, 0.7);

    vec3 H = normalize(lightDirection + viewDirection);
    float NdotH = dot(surfaceNormal, H);

    float M = sqr(roughness);
    float T = sqr(NdotH);
    return exp((T - 1)/(M * T)) / (PI * M * T * T);
}

float specularGlossy(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float power, float lightIntensity)
{
    vec3 reflected = normalize(reflect(-lightDirection, surfaceNormal));
    float dotReflectedEye = dot(reflected, viewDirection);
    return pow(max(dotReflectedEye, 0.0), power) * lightIntensity;
}

float cookTorranceGlossy(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness)
{
    viewDirection = -viewDirection;
    lightDirection = -lightDirection;

    float VdotN = max(dot(viewDirection, surfaceNormal), 0.0);
    float LdotN = max(dot(lightDirection, surfaceNormal), 0.0);

    // Half angle vector
    vec3 H = normalize(lightDirection + viewDirection);

    // Geometric term
    float NdotH = max(dot(surfaceNormal, H), 0.0);
    float VdotH = max(dot(viewDirection, H), 0.000001);
    float x = 2.0 * NdotH / VdotH;
    float G = min(1.0, min(x * VdotN, x * LdotN));

    // Distribution term
    float D = beckmannDistribution(NdotH, roughness);

    // Multiply terms and done
    return  G * D / max(PI * VdotN * LdotN, 0.000001);
}

float ggxGlossy(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness)
{
    viewDirection = -viewDirection;
    lightDirection = -lightDirection;

    vec3 H       = normalize(viewDirection + lightDirection);
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(surfaceNormal, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

/*
// Not really working...
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
*/

float fresnelDielectricCos(float cosi, float eta)
{
    /* compute fresnel reflectance without explicitly computing
    * the refracted direction */
    float c = abs(cosi);
    float g = eta * eta - 1 + c * c;
    float result;

    if (g > 0)
    {
        g = sqrt(g);
        float A = (g - c) / (g + c);
        float B = (c * (g + c) - 1) / (c * (g - c) + 1);
        result = 0.5 * A * A * (1 + B * B);
    }
    else
    result = 1.0;  /* TIR (no refracted component) */

    return result;
}

float fresnel(float IOR)
{
    float dotViewNormal = max(dot(-eyeDirection, surfaceNormal), 0.0);
    return clamp(pow((1.0 - dotViewNormal) * (IOR * 2.0), 2.0), 0.0, 1.0);
}

float _fresnel(float IOR)
{
    float f = max(IOR, 1e-5);
    // float eta = backfacing() ? 1.0 / f : f;
    float eta = 1.0 / f;
    float cosi = dot(-eyeDirection, surfaceNormal);
    return clamp(fresnelDielectricCos(cosi, eta), 0.0, 1.0);
}

float SchlickFresnel(float u)
{
    float m = clamp(1-u, 0, 1);
    float m2 = m * m;
    return m2 * m2 * m; // pow(m,5)
}

float GTR1(float NdotH, float a)
{
    if (a >= 1) return 1/PI;
    float a2 = a*a;
    float t = 1 + (a2-1)*NdotH*NdotH;
    return (a2-1) / (PI*log(a2)*t);
}

float GTR2(float NdotH, float a)
{
    float a2 = a*a;
    float t = 1 + (a2-1)*NdotH*NdotH;
    return a2 / (PI * t * t);
}

float GTR2_aniso(float NdotH, float HdotX, float HdotY, float ax, float ay)
{
    return 1 / (PI * ax*ay * sqr( sqr(HdotX/ax) + sqr(HdotY/ay) + NdotH*NdotH ));
}

float smithG_GGX(float NdotV, float alphaG)
{
    float a = alphaG*alphaG;
    float b = NdotV*NdotV;
    return 1 / (NdotV + sqrt(a + b - a*b));
}

float smithG_GGX_aniso(float NdotV, float VdotX, float VdotY, float ax, float ay)
{
    return 1 / (NdotV + sqrt( sqr(VdotX*ax) + sqr(VdotY*ay) + sqr(NdotV) ));
}

vec3 mon2lin(vec3 x)
{
    return vec3(pow(x[0], 2.2), pow(x[1], 2.2), pow(x[2], 2.2));
}

vec3 disney(
        vec3 L,
        vec3 V,
        vec3 N,
        vec3 X,
        vec3 Y,
        vec3 baseColor,                 // .82 .67 .16
        float metallic,                 // 0 1 0
        float subsurface,               // 0 1 0
        float specular,                 // 0 1 .5
        float roughness,                // 0 1 .5
        float specularTint,             // 0 1 0
        float anisotropic,              // 0 1 0
        float sheen,                    // 0 1 0
        float sheenTint,                // 0 1 .5
        float clearcoat,                // 0 1 0
        float clearcoatGloss            // 0 1 1
        )
{
    L = -L;
    V = -V;

    float NdotL = dot(N,L);
    float NdotV = dot(N,V);
    if (NdotL < 0 || NdotV < 0) return vec3(0);

    vec3 H = normalize(L+V);
    float NdotH = dot(N,H);
    float LdotH = dot(L,H);

    vec3 Cdlin = mon2lin(baseColor);
    float Cdlum = .3*Cdlin[0] + .6*Cdlin[1]  + .1*Cdlin[2]; // luminance approx.

    vec3 Ctint = Cdlum > 0 ? Cdlin/Cdlum : vec3(1); // normalize lum. to isolate hue+sat
    vec3 Cspec0 = mix(specular * 0.08 * mix(vec3(1), Ctint, specularTint), Cdlin, metallic);
    vec3 Csheen = mix(vec3(1), Ctint, sheenTint);

    // Diffuse fresnel - go from 1 at normal incidence to .5 at grazing
    // and mix in diffuse retro-reflection based on roughness
    float FL = SchlickFresnel(NdotL), FV = SchlickFresnel(NdotV);
    float Fd90 = 0.5 + 2 * LdotH * LdotH * roughness;
    float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);

    // Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
    // 1.25 scale is used to (roughly) preserve albedo
    // Fss90 used to "flatten" retroreflection based on roughness
    float Fss90 = LdotH * LdotH * roughness;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
    float ss = 1.25 * (Fss * (1 / (NdotL + NdotV) - 0.5) + 0.5);

    // specular
    float aspect = sqrt(1.0 - anisotropic * 0.9);
    float ax = max(0.001, sqr(roughness) / aspect);
    float ay = max(0.001, sqr(roughness) * aspect);
    float Ds = GTR2_aniso(NdotH, dot(H, X), dot(H, Y), ax, ay);
    float FH = SchlickFresnel(LdotH);
    vec3 Fs = mix(Cspec0, vec3(1), FH);
    float Gs;
    Gs  = smithG_GGX_aniso(NdotL, dot(L, X), dot(L, Y), ax, ay);
    Gs *= smithG_GGX_aniso(NdotV, dot(V, X), dot(V, Y), ax, ay);

    // sheen
    vec3 Fsheen = FH * sheen * Csheen;

    // clearcoat (ior = 1.5 -> F0 = 0.04)
    float Dr = GTR1(NdotH, mix(.1,.001,clearcoatGloss));
    float Fr = mix(.04, 1.0, FH);
    float Gr = smithG_GGX(NdotL, .25) * smithG_GGX(NdotV, .25);

    if (passType == PASS_ENV)
    {
        Fd = 0.0;
    }

    return ((1.0 / PI) * mix(Fd, ss, subsurface) * Cdlin + Fsheen) * (1.0 - metallic) + Gs * Fs * Ds + 0.25 * clearcoat * Gr * Fr * Dr;
}

//-------------------------------------------------------------------------------------------------
// Perlin noise

float perlinHash(vec3 p)
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
                mix(mix(perlinHash(i + vec3(0.0, 0.0, 0.0)), perlinHash(i + vec3(1.0, 0.0, 0.0)),f.x),
                    mix(perlinHash(i + vec3(0.0, 1.0, 0.0)), perlinHash(i + vec3(1.0, 1.0, 0.0)),f.x),
                    f.y),
                mix(mix(perlinHash(i + vec3(0.0, 0.0, 1.0)), perlinHash(i + vec3(1.0, 0.0, 1.0)),f.x),
                    mix(perlinHash(i + vec3(0.0, 1.0, 1.0)), perlinHash(i + vec3(1.0, 1.0, 1.0)),f.x),
                    f.y),
                f.z);

    return clamp((n - 0.49) * 2.05, -1.0, 1.0);
}

// Computes a perlin noise value between 0 and 1 for p
float perlin01(vec3 p)
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
        f = f * 2.0 + perlin01(pos);
        pos *= 1.5;
    }

    f /= pow(2.0, float(TURBULENCE_OCTAVES));
    return f;
}

//-------------------------------------------------------------------------------------------------

float getHeightForNormalMap(vec2 position)
{
    vec4 color = texture2D(bumpMap, position);
    float z = ((color.r + color.g + color.b) / 3.0) * bumpHeight;
    return z;
}

float getHeight(vec2 position)
{
    float z = getHeightForNormalMap(position);
    if (z > 0.0) z += bumpAltitude;
    return z;
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
        vec2 noiseVec3 = vec2(start.x * 200.0 + float(areaIndex), start.y * 200.0 - float(areaIndex));
        float offsetX = (1.0 - (rand(noiseVec1) * 2.0)) * (lightAreaRadius * 0.5);
        float offsetY = (1.0 - (rand(noiseVec2) * 2.0)) * (lightAreaRadius * 0.5);
        float offsetZ = (1.0 - (rand(noiseVec3) * 2.0)) * (lightAreaRadius * 0.5);
        vec3 areaPosition = finish + vec3(offsetX, offsetY, offsetZ);
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

            if (position.z > bumpAltitude && position.z < obstructionZ)
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
// Shade

// %shade%

//-------------------------------------------------------------------------------------------------
// Lighting

vec4 lightUp()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

    //-----------------------------------------------
    // Compute bump normal

    float me = position.z;
    float n = getHeightForNormalMap(position.xy + vec2(0.0, -pixelDistanceY)) * 20.0;
    float s = getHeightForNormalMap(position.xy + vec2(0.0,  pixelDistanceY)) * 20.0;
    float e = getHeightForNormalMap(position.xy + vec2( pixelDistanceX, 0.0)) * 20.0;
    float w = getHeightForNormalMap(position.xy + vec2(-pixelDistanceX, 0.0)) * 20.0;

    if (n > 0.0 && s > 0.0 && e > 0.0 && w > 0.0)
    {
        // find perpendicular vector to normal
        vec3 temp = surfaceNormal;
        if (surfaceNormal.x == 1) temp.y += 0.5; else temp.x += 0.5;

        // form a basis with normal being one of the axes
        vec3 perp1 = normalize(cross(surfaceNormal, temp));
        vec3 perp2 = normalize(cross(surfaceNormal, perp1));

        // use the basis to move the normal in its own space by the offset
        vec3 normalOffset = -1.0 * (((n - me) - (s - me)) * perp1 + ((e - me) - (w - me)) * perp2);
        surfaceNormal = normalize(surfaceNormal - normalOffset);

        // find perpendicular vector to normal
        temp = surfaceNormal;
        if (surfaceNormal.x == 1) temp.y += 0.5; else temp.x += 0.5;
        surfaceNormalTangent = normalize(cross(surfaceNormal, temp));
        surfaceNormalBitangent = normalize(cross(surfaceNormal, surfaceNormalTangent));
    }

    facing = clamp(max(dot(surfaceNormal, -eyeDirection), 0.0), 0.0, 1.0);
    facingInverse = 1.0 - facing;

    //-----------------------------------------------
    // Iterate through all lights

    // for (int index = 0; index < u_num_lights; index++)
    {
        float dist = distance(position, lightPosition);

        if (lightDistance == 0.0 || dist < lightDistance)
        {
            // Light ray
            if (lightIsDirectional)
            {
                lightRay = lightDirection;
            }
            else
            {
                lightRay = normalize(position - lightPosition);
            }

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
                float spotEffect = (dot(normalize(lightDirection), lightRay));
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
                if (lightShadowSampleCount > 0)
                {
                    if (lightIsDirectional)
                    {
                        attenuation *= rayMarchShadows(position, position - lightRay);
                    }
                    else
                    {
                        attenuation *= rayMarchShadows(position, lightPosition);
                    }
                }

                passType = PASS_LIGHT;

                vec4 shadeResult = shade();

                color.rgb += (lightColor.rgb * shadeResult.rgb * attenuation * lightIntensity) * shadeResult.a;
                color.a = max(color.a, shadeResult.a);
            }
        }
    }

    //-----------------------------------------------
    // Iterate through environment

    if (environmentSampleCount > 0)
    {
        int numEnvSamplesProcessed = 0;

        vec4 accumEnvironmentColor = vec4(0.0, 0.0, 0.0, 0.0);

        passType = PASS_ENV;

        for (int y = 0; y < environmentSampleCount; y++)
        {
            for (int x = 0; x < environmentSampleCount; x++)
            {
                // Compute normalized x and y from -1.0 to 1.0
                float normX = ((float(x) / float(environmentSampleCount)) - 0.5) * 2.0;
                float normY = ((float(y) / float(environmentSampleCount)) - 0.5) * 2.0;

                // Rotate surface normal by some angles
                vec4 quat1 = quaternionFromAxisAndAngle(1.0, 0.0, 0.0, normX * 0.5 * PI);
                vec4 quat2 = quaternionFromAxisAndAngle(0.0, 0.0, 1.0, normY * 0.5 * PI);
                lightRay = rotateVectorFast(quat1, -surfaceNormal);
                lightRay = rotateVectorFast(quat2, lightRay);

                // lightRay = -normalize(surfaceNormal + vec3(normX * 0.1, normY * 0.1, 0.0));

                // lightRay = -surfaceNormal;

                // Add a bit of pixel position to UV and get environment color
                vec2 tileUV = (position.xy - 0.5) * 0.1;
                vec2 environmentUV = mapDirectionVectorToEqui(-lightRay) + tileUV;

                if (environmentUV.x >= 0.0 && environmentUV.x <= 1.0 && environmentUV.y >= 0.0 && environmentUV.y <= 1.0)
                {
                    vec4 environmentColor = texture2D(environmentMap, environmentUV);

                    vec4 shadeResult = shade();

                    float shadeResultGray = colorLuminosity(shadeResult);

                    accumEnvironmentColor.rgb += environmentColor.rgb * vec3(shadeResultGray, shadeResultGray, shadeResultGray);
                    numEnvSamplesProcessed++;
                }
            }
        }

        if (numEnvSamplesProcessed > 0)
        {
            accumEnvironmentColor.r /= float(numEnvSamplesProcessed);
            accumEnvironmentColor.g /= float(numEnvSamplesProcessed);
            accumEnvironmentColor.b /= float(numEnvSamplesProcessed);

            color.rgb += accumEnvironmentColor.rgb;

            // color.r = max(color.r, envColor.r);
            // color.g = max(color.g, envColor.g);
            // color.b = max(color.b, envColor.b);
        }
    }

    // Return material ambient plus light color

    color.r = max(materialAmbientColor.r, color.r);
    color.g = max(materialAmbientColor.g, color.g);
    color.b = max(materialAmbientColor.b, color.b);

    return vec4(color.rgb, color.a);
}

//-------------------------------------------------------------------------------------------------
// Lens effects

vec3 flareDisc(vec3 light, float where)
{
    vec3 center = vec3(0.5, 0.5, 0.0);
    vec3 ray = light - center;
    return center - ray * where;
}

vec4 lightRays()
{
    float final = 0.0;

    if (lightPosition.x >= 0.0 && lightPosition.x <= 1.0 && lightPosition.y >= 0.0 && lightPosition.y <= 1.0)
    {
        float diffX = abs(position.x - lightPosition.x) * 0.25;
        float rayAmountX = 1.0 - clamp(diffX, 0.0, 1.0);
        float diffY = abs(position.y - lightPosition.y) * 1.0;
        float rayAmountY = 1.0 - clamp(diffY, 0.0, 1.0);
        final = pow(rayAmountX * rayAmountY, 20.0);
    }

    final *= lightIntensity * lightFlareIntensity;

    return vec4(lightColor.rgb, final);
}

float flareType1(float input)
{
    return clamp(impulse(8.0, input), 0.0, 1.0);
}

float flareType2(float input)
{
    return clamp(cubicPulse(0.5, 0.5, input), 0.0, 1.0);
}

vec4 lightFlares()
{
    float final = 0.0;

    if (lightPosition.x >= 0.0 && lightPosition.x <= 1.0 && lightPosition.y >= 0.0 && lightPosition.y <= 1.0)
    {
        final += clamp(flareType1(disc(flareDisc(vec3(lightPosition.xy, 0.0), 0.2), 0.06, position)), 0.0, 1.0);
        final += clamp(flareType1(disc(flareDisc(vec3(lightPosition.xy, 0.0), 0.3), 0.03, position)), 0.0, 1.0);
        final += clamp(flareType2(disc(flareDisc(vec3(lightPosition.xy, 0.0), 0.4), 0.03, position)), 0.0, 1.0);
        final += clamp(flareType2(disc(flareDisc(vec3(lightPosition.xy, 0.0), 0.7), 0.10, position)), 0.0, 1.0);
    }

    final *= lightIntensity * lightFlareIntensity * 0.5;

    return vec4(lightColor.rgb, final);
}

//-------------------------------------------------------------------------------------------------
// Main

void main()
{
    position = vec3(qt_TexCoord0.x, qt_TexCoord0.y, getHeight(qt_TexCoord0));
    surfaceNormal = up;
    surfaceNormalTangent = right;
    surfaceNormalBitangent = rear;
    eyePosition = vec3(0.5, 0.5, 10.0);
    // eyeDirection = normalize(position - eyePosition);
    eyeDirection = down;

    // Get diffuse color
    materialDiffuseColor = texture2D(source, position.xy);

    // Get material contribution
    vec4 materialLight = lightUp();

    // Get lens effects contribution
    vec4 lensLight = lightRays() + lightFlares();

    // Compose final color
    vec4 finalColor = mix(materialLight, lensLight, lensLight.a);
    finalColor.a = max(materialLight.a, lensLight.a);

    // Uncomment to see light position
    // finalColor.rgb += vec3(1.0, 0.0, 0.0) * flatDisc(vec3(lightPosition.xy, 0.0), 0.01, vec3(position.xy, 0.0));

    gl_FragColor = vec4(finalColor.rgb, finalColor.a * qt_Opacity);

    // Uncomment to visualize a vector
    // gl_FragColor = vec4((position.rgb + 1.0) / 2.0, 1.0);
    // gl_FragColor = vec4((eyePosition.rgb + 1.0) / 2.0, 1.0);
    // gl_FragColor = vec4((eyeDirection.rgb + 1.0) / 2.0, 1.0);
    // gl_FragColor = vec4((surfaceNormal.rgb + 1.0) / 2.0, 0.0);
}
