vec4 shade()
{
    float fresnelTerm1 = fresnel(10.0);
    float fresnelTerm2 = fresnel(5.0);
    vec3 goldColor = vec3(1.00, 0.88, 0.25) * materialDiffuseColor.rgb;
    float diffuseFactor = orenNayarDiffuse(lightRay, eyeDirection, surfaceNormal, 0.1, 2.0);
    float glossyFactor = cookTorranceGlossy(lightRay, eyeDirection, surfaceNormal, 0.1);
    vec3 metal1 = goldColor * diffuseFactor * fresnelTerm1;
    vec3 metal2 = goldColor * glossyFactor * fresnelTerm2;
    return vec4(mix(metal1, metal2, 0.8), 1.0);
}
