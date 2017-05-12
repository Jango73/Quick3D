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
