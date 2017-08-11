vec4 shade()
{
    float fresnelTerm = fresnel(2.0);
    float diffuseFactor = orenNayarDiffuse(lightRay, eyeDirection, surfaceNormal, 1.0, 2.0);
    float glossyFactor = ggxGlossy(lightRay, eyeDirection, surfaceNormal, 0.2) * fresnelTerm;
    return vec4(mix(materialDiffuseColor.rgb * diffuseFactor, materialGlossyColor.rgb * glossyFactor, 0.6), 1.0);
}
