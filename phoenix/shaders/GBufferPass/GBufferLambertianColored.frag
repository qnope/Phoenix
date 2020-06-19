#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(early_fragment_tests) in;

layout(location = 0) in VertexData {
    vec3 normal;
    vec3 tangeant;
    vec3 bitangeant;
    vec2 texCoord;
} inData;

layout(location = 0) out vec4 outColor;

struct LambertianMaterial {
    vec4 color;
};

layout(set = 1, binding = 0) uniform buffer MaterialBuffer {
    LambertianMaterial materials[];
};

layout(push_constant) uniform MaterialIndex {
    uint materialIndex;
};

void main() {
    outColor = materials[materialIndex];
}
