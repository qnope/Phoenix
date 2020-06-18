#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(push_constant) uniform MatrixIndex {
    uint matrixIndex;
};

layout(set = 0, binding = 0, std430) buffer MatrixBuffer {
    mat4 matrices[];
};

void main() {
    gl_Position = matrices[matrixIndex] * vec4(inPosition, 1.0);
}
