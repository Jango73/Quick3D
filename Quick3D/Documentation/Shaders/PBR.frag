
uniform mat4 viewMatrix;

uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform float lightDistance;
uniform float lightOuterAngle;
uniform bool lightIsDirectional;

uniform vec4 diffuseColor;
uniform vec4 ambientColor;
uniform float metallic;
uniform float roughness;
uniform float glossy;
uniform float emission;
uniform bool environmentOnly;
uniform int environmentSampleCount;
uniform float normalMapAmount;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D environmentMap;

uniform sampler2DShadow shadowMapTexture;

in vec4 positionInLightSpace;
in vec3 position;
in vec3 normal;
in vec3 normalTangent;
in vec3 normalBitangent;
in vec2 texCoord;
in mat3 tangentMatrix;
in mat3 tangentMatrixInverse;

out vec4 fragColor;

const float PI = 3.14159265358979323846264;
const float _2PI = (PI * 2.0);
const int PASS_LIGHT = 0;
const int PASS_ENV = 1;

vec3 lightPositionInViewSpace;
vec3 eyeDirection;
vec3 surfaceNormal;
vec3 surfaceNormalTangent;
vec3 surfaceNormalBitangent;
vec3 lightRay;
vec4 diffuseColorToUse;
int passType;
float facing;
float facingInverse;

//-------------------------------------------------------------------------------------------------
// Math functions

float sqr(float x)
{
    return x * x;
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
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

float fresnel(float IOR)
{
    float NdotL = max(0.0, dot(surfaceNormal, eyeDirection));
    float result = IOR + (1.0 - IOR) * pow((1.0 - NdotL), 5.0);
    return clamp(result, 0.0, 1.0);
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
// BRDF functions

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
    if (emission >= 1)
    {
        return baseColor;
    }

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

    vec3 Total = ((1.0 / PI) * mix(Fd, ss, subsurface) * Cdlin + Fsheen) * (1.0 - metallic) + Gs * Fs * Ds + 0.25 * clearcoat * Gr * Fr * Dr;

    return mix(Total, baseColor, emission);
}

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

    viewDirection = -viewDirection;

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

    viewDirection = -viewDirection;
    lightDirection = -lightDirection;

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
    viewDirection = -viewDirection;
    lightDirection = -lightDirection;

    vec3 reflected = normalize(reflect(-lightDirection, surfaceNormal));
    float dotReflectedEye = dot(reflected, viewDirection);
    return pow(max(dotReflectedEye, 0.0), power) * lightIntensity;
}

float cookTorranceGlossy(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness)
{
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

vec4 shade()
{
//    return vec4(
//                disney(lightRay, eyeDirection, surfaceNormal,
//                     surfaceNormalTangent, surfaceNormalBitangent,
//                     diffuseColorToUse.rgb,                         // baseColor
//                     metallic,                                      // metallic
//                     0.0,                                           // subsurface
//                     glossy,                                        // specular
//                     roughness,                                     // roughness
//                     0.0,                                           // specularTint
//                     0.0,                                           // anisotropic
//                     0.0,                                           // sheen
//                     0.0,                                           // sheenTint
//                     0.0,                                           // clearcoat
//                     0.0                                            // clearcoatGloss
//                     )
//                , 1.0);

    float fresnelTerm = fresnel(0.2);

    float diffuseFactor1 = orenNayarDiffuse(lightRay, eyeDirection, surfaceNormal, roughness, 1.0);
    float glossyFactor1 = cookTorranceGlossy(lightRay, eyeDirection, surfaceNormal, roughness) * fresnelTerm;
    vec3 total1 = mix(diffuseColorToUse.rgb * diffuseFactor1, vec3(1.0, 1.0, 1.0) * glossyFactor1, glossy);

    float diffuseFactor2 = orenNayarDiffuse(lightRay, eyeDirection, surfaceNormal, roughness, 1.0);
    float glossyFactor2 = cookTorranceGlossy(lightRay, eyeDirection, surfaceNormal, roughness) * fresnelTerm;
    vec3 total2 = mix(diffuseColorToUse.rgb * diffuseFactor2, diffuseColorToUse.rgb * glossyFactor2, glossy);

    vec3 total = mix(total1, total2, metallic);
    vec3 emitted = mix(total, diffuseColorToUse, emission);

    return vec4(emitted, 1.0);
}

vec4 environment()
{
//    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

//    if (environmentSampleCount > 0)
//    {
//        int numEnvSamplesProcessed = 0;

//        vec4 accumEnvironmentColor = vec4(0.0, 0.0, 0.0, 0.0);

//        passType = PASS_ENV;

//        for (int y = 0; y < environmentSampleCount; y++)
//        {
//            for (int x = 0; x < environmentSampleCount; x++)
//            {
//                // Compute normalized x and y from -1.0 to 1.0
//                float normX = ((float(x) / float(environmentSampleCount)) - 0.5) * 2.0;
//                float normY = ((float(y) / float(environmentSampleCount)) - 0.5) * 2.0;

//                // Rotate surface normal by some angles
//                vec4 quat1 = quaternionFromAxisAndAngle(1.0, 0.0, 0.0, normX * 0.5 * PI);
//                vec4 quat2 = quaternionFromAxisAndAngle(0.0, 1.0, 0.0, normY * 0.5 * PI);
//                lightRay = rotateVectorFast(quat1, vec3(0.0, 0.0, 1.0));
//                lightRay = rotateVectorFast(quat2, lightRay);
//                lightRay = tangentMatrixInverse * lightRay;

//                // Add a bit of pixel position to UV and get environment color
//                vec2 environmentUV = mapDirectionVectorToEqui(-lightRay);

//                vec4 environmentColor = texture2D(environmentMap, environmentUV);

//                vec4 shadeResult = shade();

//                float shadeResultGray = colorLuminosity(shadeResult);

//                color.rgb += environmentColor.rgb * vec3(shadeResultGray, shadeResultGray, shadeResultGray);
//                numEnvSamplesProcessed++;
//            }
//        }

//        if (numEnvSamplesProcessed > 0)
//        {
//            color.r /= float(numEnvSamplesProcessed);
//            color.g /= float(numEnvSamplesProcessed);
//            color.b /= float(numEnvSamplesProcessed);
//        }
//    }

//    return color;

    if (environmentSampleCount > 0)
    {
        float fresnelTerm = fresnel(0.2);
        vec2 environmentUV = mapDirectionVectorToEqui(surfaceNormal);
        vec4 environmentColor = texture2D(environmentMap, environmentUV);
        return vec4(environmentColor.rgb * fresnelTerm * glossy, 1.0);
    }

    return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 lightUp()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    if (environmentOnly)
    {
        return environment();
    }

    //-----------------------------------------------
    // Modify normal

    vec4 sampledNormalColor = texture2D(normalMap, texCoord);

    if (sampledNormalColor.a > 0.0)
    {
        vec3 newNormal = tangentMatrixInverse * vec3(sampledNormalColor.rgb);
        surfaceNormal = mix(surfaceNormal, newNormal, normalMapAmount);
    }

    facing = clamp(max(dot(surfaceNormal, -eyeDirection), 0.0), 0.0, 1.0);
    facingInverse = 1.0 - facing;

    //-----------------------------------------------
    // Iterate through all lights

    // for (int index = 0; index < u_num_lights; index++)
    {
        float dist = distance(position, lightPositionInViewSpace);

        if (lightDistance == 0.0 || dist < lightDistance)
        {
            // Light ray
            if (lightIsDirectional)
            {
                lightRay = lightDirection;
            }
            else
            {
                lightRay = normalize(lightPositionInViewSpace - position);
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
                if (lightIsDirectional)
                {
                    // attenuation *= rayMarchShadows(position, position - lightRay);
                }
                else
                {
                    float shadow = textureProj(shadowMapTexture, positionInLightSpace);
                    attenuation *= shadow;
                }

                if (attenuation > 0.0)
                {
                    passType = PASS_LIGHT;

                    vec4 shadeResult = shade();

                    color.rgb += (lightColor.rgb * shadeResult.rgb * attenuation * lightIntensity) * shadeResult.a;
                    color.a = max(color.a, shadeResult.a);
                }
            }
        }
    }

    //-----------------------------------------------
    // Get environment contribution

    color.rgb += environment().rgb;

    // Return material ambient plus light color

    color.r = max(ambientColor.r, color.r);
    color.g = max(ambientColor.g, color.g);
    color.b = max(ambientColor.b, color.b);

    return vec4(color.rgb, color.a);
}

void main()
{
    eyeDirection = normalize(-position);

    surfaceNormal = normalize(normal);
    surfaceNormalTangent = normalTangent;
    surfaceNormalBitangent = normalBitangent;

    lightPositionInViewSpace = vec3(viewMatrix * vec4(lightPosition, 1.0));

    vec4 sampledDiffuseColor = texture2D(diffuseMap, texCoord);

    float alpha = max(diffuseColor.a, sampledDiffuseColor.a);

    diffuseColorToUse = vec4(mix(diffuseColor.rgb, sampledDiffuseColor.rgb, sampledDiffuseColor.a), alpha);

    fragColor = lightUp();
}
