#version 450 core
#extension GL_ARB_separate_shader_objects : enable

layout(early_fragment_tests) in;

layout(location = 0) in vec2 ndcCoord;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform SkyInformations {
    mat4 inversedMatrix;
};

const float radiusPlanet = 6360e3;
const float radiusAtomosphere = 6420e3;

const float HrRayleigh = 8000;
const float HrMie = 1200;

const vec3 betaRayleigh = 1.0 * vec3(5.8e-6, 13.5e-6, 33.1e-6);

const vec3 betaMie = vec3(210e-5, 210e-5, 210e-5);
const float gMie = 0.76;

const float pi = 3.1415926535;

const vec3 sunDir = normalize(vec3(.0, 1.0, 0.0));

const uint NUM_SAMPLE = 100;
const uint NUM_SAMPLE_LIGHT = 10;

float phaseRayleigh(float u) {
    return (3.0 / 16.0 * pi) * (1.0 + u * u);
}

float phaseMie(float u) {
    const float g = gMie;
    const float g2 = g * g;
    float piFactor = 3.0 / (8.0 * pi);
    float numerator = (1.0 - g2) * (1.0 + u * u);
    float denominator = (2.0 + g2) * pow(1.0 + g2 - 2.0 * g * u, 3.0 / 2.0);
    return piFactor * numerator / denominator;
}

vec3 getWorldVectorFromDepth(float depth) {
    vec4 ndcSpace = vec4(ndcCoord, depth, 1.0);
    vec4 result = inversedMatrix * ndcSpace;
    return (result / result.w).xyz;
}

float intersectAtmosphere(vec3 orig, vec3 dir) {
    float radiusSquare = radiusAtomosphere * radiusAtomosphere;
    float b = dot(dir, orig);
    float c = dot(orig, orig) - radiusSquare;
    float delta = b * b - c;

    if(delta < 0.0)
        return -1.0;

    float disc = sqrt(delta);
    return -b + disc;
}

void main() {
    vec3 origin = getWorldVectorFromDepth(0.0);
    vec3 far = getWorldVectorFromDepth(1.0);
    vec3 dir = normalize(far - origin);

    origin += vec3(0.0, radiusPlanet, 0.0);
    float distanceToEndAtmosphere = intersectAtmosphere(origin, dir);
    float segmentLength = distanceToEndAtmosphere / NUM_SAMPLE;
    far = origin + distanceToEndAtmosphere * dir;

    float u = dot(dir, sunDir);
    float opticalDepthR = 0.0;
    float opticalDepthM = 0.0;

    vec3 sumR = vec3(0.0);
    vec3 sumM = vec3(0.0);
    for(uint i = 1; i <= NUM_SAMPLE; ++i) {
        vec3 samplePosition = origin + (dir * segmentLength * i);
        float height = length(samplePosition) - radiusPlanet;
        float hr = exp(-height / HrRayleigh) * segmentLength;
        float hm = exp(-height / HrMie) * segmentLength;
        opticalDepthR += hr;
        opticalDepthM += hm;

        float distanceToSun = intersectAtmosphere(samplePosition, sunDir);
        float segmentLengthSun = distanceToSun / NUM_SAMPLE_LIGHT;

        float opticalDepthSunR = 0.0;
        float opticalDepthSunM = 0.0;

        for(uint j = 1; j <= NUM_SAMPLE_LIGHT; ++j) {
            vec3 samplePositionSun = samplePosition + (sunDir * segmentLengthSun * j);
            float heightLight = length(samplePositionSun) - radiusPlanet;
            opticalDepthSunR += exp(-heightLight / HrRayleigh) * segmentLengthSun;
            opticalDepthSunM += exp(-heightLight / HrMie) * segmentLengthSun;
        }

        vec3 tau = betaRayleigh * (opticalDepthR + opticalDepthSunR) + betaMie * 1.1f * (opticalDepthM + opticalDepthSunM);
        vec3 attenuation = exp(-tau);
        sumR += attenuation * hr;
        sumM += attenuation * hm;
    }

    outColor.a = 1.0;

    outColor.rgb = (sumR * betaRayleigh * phaseRayleigh(u) + sumM * betaMie * phaseMie(u)) * 20.0;

    outColor.rgb = 1.0 - exp(-outColor.rgb);
}

