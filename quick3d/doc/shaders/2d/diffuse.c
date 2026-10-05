vec4 shade()
{
    float diffuseFactor = beckmannDiffuse(lightRay, eyeDirection, surfaceNormal, 0.6);
    return vec4(materialDiffuseColor.rgb * diffuseFactor, 1.0);
}
