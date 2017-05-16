//-------------------------------------------------------------------------------------------------
// BRDFs (Bidirectional reflectance distribution function)

float beckmannDistribution(float x, float roughness)
{
    float NdotH = max(x, 0.0001);
    float cos2Alpha = NdotH * NdotH;
    float tan2Alpha = (cos2Alpha - 1.0) / cos2Alpha;
    float roughness2 = roughness * roughness;
    float denom = 3.141592653589793 * roughness2 * cos2Alpha * cos2Alpha;
    return exp(tan2Alpha / roughness2) / denom;
}

float phongDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal)
{
    return max(dot(surfaceNormal, lightDirection), 0.0);
}

float orenNayarDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, float albedo)
{
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

float specularGlossy(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float lightIntensity)
{
    vec3 reflected = normalize(reflect(lightDirection, surfaceNormal));
    float dotReflectedEye = dot(reflected, viewDirection);
    return pow(max(dotReflectedEye, 0.0), materialGlossyShininess) * lightIntensity;
}

float cookTorranceGlossy(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, float fresnel)
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

    // Fresnel term
    float F = pow(1.0 - VdotN, fresnel);

    // Multiply terms and done
    return  G * F * D / max(3.14159265 * VdotN * LdotN, 0.000001);
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

float fresnel(vec3 viewDirection, vec3 surfaceNormal, float ior)
{
    float dotViewNormal = max(dot(-viewDirection, surfaceNormal), 0.0);
    return pow((1.0 - dotViewNormal) * (ior * 2.0), 2.0);
}
