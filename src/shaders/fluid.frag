#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(binding = 1) uniform sampler2D texSampler[];

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main()
{
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (dot(circCoord, circCoord) > 1.0) discard;
    outColor = fragColor;
}