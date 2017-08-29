uniform sampler2D source;
uniform lowp float qt_Opacity;
varying highp vec2 qt_TexCoord0;

uniform float resolutionX;
uniform float resolutionY;

vec4 blur(sampler2D image, vec2 uv, vec2 resolution, vec2 direction)
{
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(1.411764705882353) * direction;
    vec2 off2 = vec2(3.2941176470588234) * direction;
    vec2 off3 = vec2(5.176470588235294) * direction;
    color += texture2D(image, uv) * 0.1964825501511404;
    color += texture2D(image, uv + (off1 / resolution)) * 0.2969069646728344;
    color += texture2D(image, uv - (off1 / resolution)) * 0.2969069646728344;
    color += texture2D(image, uv + (off2 / resolution)) * 0.09447039785044732;
    color += texture2D(image, uv - (off2 / resolution)) * 0.09447039785044732;
    color += texture2D(image, uv + (off3 / resolution)) * 0.010381362401148057;
    color += texture2D(image, uv - (off3 / resolution)) * 0.010381362401148057;
    return color;
}

vec4 contrastSaturationBrightness(vec4 color, float contrast, float saturation, float brightness)
{
    vec3 newColor = mix(vec3(0.5), mix(vec3(dot(vec3(0.2125, 0.7154, 0.0721), color.rgb * brightness)), color.rgb * brightness, saturation), contrast);

    return vec4(newColor.rgb, color.a);
}

void main()
{
    vec4 sourceColor = texture2D(source, qt_TexCoord0.xy);
    vec2 direction = vec2(1.5, 0.0);
    vec4 addColor = contrastSaturationBrightness(blur(source, qt_TexCoord0, vec2(resolutionX, resolutionY), direction), 1.25, 1.0, 0.5);

    gl_FragColor = sourceColor + addColor;
}
