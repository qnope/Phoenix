#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangeant;
layout(location = 3) in vec3 inBitangeant;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out VertexData {
    vec3 normal;
    vec3 tangeant;
    vec3 bitangeant;
    vec2 texCoord;
} outData;

void main() {
    gl_Position = vec4(inPosition, 1.0);

    outData.normal = inNormal;
    outData.tangeant = inTangeant;
    outData.texCoord = inTexCoord;
    outData.bitangeant = inBitangeant;
}
