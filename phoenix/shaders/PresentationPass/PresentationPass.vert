#version 450 core
#extension GL_ARB_separate_shader_objects : enable

const vec2 inPosition[4] = vec2[4](
            vec2(-1.0, -1.0),
            vec2(-1.0, 1.0),
            vec2(1.0, -1.0),
            vec2(1.0, 1.0));

layout(location = 0) out vec2 texCoord;

void main() {
    gl_Position = vec4(inPosition[gl_VertexIndex], 0.0, 1.0);
    texCoord = inPosition[gl_VertexIndex] * 0.5 + 0.5;
}
