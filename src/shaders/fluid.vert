#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform frame_buffer_object {
    mat4 viewProj;
} frameBuffer;

layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);
    gl_PointSize = 5;
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}