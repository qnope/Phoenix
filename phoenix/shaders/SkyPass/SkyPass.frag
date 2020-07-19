#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(early_fragment_tests) in;

layout(location = 0) in vec2 ndcCoord;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform SkyInformations {
    vec4 cameraPosition;
};

const float radiusPlanet = 6360000;
const float radiuAtomosphere = 6420000;

const float HrRayleigh = 8000;
const float HrMie = 1200;

const vec3 betaRayleigh = vec3(33.1e-6, 13.5e-6, 5.8e-6);

const vec3 betaMie = vec3(210e-5, 210e-5, 210e-5);
const float gMie = 0.76;

const float pi = 3.1415926535;

float phaseRayleigh(float u) {
    return 3.0 / pi * (1.0 + u * u);
}

float phaseMie(float u) {
    const float g = gMie;
    const float g2 = g * g;
    float piFactor = 3.0 / (8.0 * pi);
    float numerator = (1.0 - g2) * (1.0 + u * u);
    float denominator = (2.0 + g2) * pow(1.0 + g2 - 2.0 * g * u, 3.0 / 2.0);
    return piFactor * numerator / denominator;
}

void main() {
    outColor.rgb = betaRayleigh * 100000;
    outColor.a = 1.0;
}
