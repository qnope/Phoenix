#version 450 core
#extension GL_ARB_separate_shader_objects : enable

const vec2 vertices[4] = vec2[4](
            vec2(-1.0, -1.0),
            vec2(1.0, -1.0),
            vec2(-1.0, 1.0),
            vec2(1.0, 1.0));

layout(location = 0) out vec2 ndcCoord;

void main() {
    gl_Position = vec4(vertices[gl_VertexIndex], 1.0, 1.0);
    ndcCoord = vertices[gl_VertexIndex];
}
