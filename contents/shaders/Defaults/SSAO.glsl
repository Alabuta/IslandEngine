/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: screen space ambient oclusion shader.
****
********************************************************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#pragma stage("vertex")

layout(location = nPOSITION) in vec3 inVertex;
layout(location = nNORMAL) in vec3 inNormal;
layout(location = nTEX_COORD) in vec2 inTexCoord;

/*layout(location = 0)*/ uniform vec4 lightPosition = vec4(10, 10, 10, 0);

subroutine void RenderPassType();
layout(location = 0) subroutine uniform RenderPassType RenderPass;

out vec4 light;
out vec4 normal;
out vec2 texCoord;

out vec3 ray;

layout(index = 0) subroutine(RenderPassType)
void modelProccessing()
{
    texCoord = inTexCoord;

    normal = normalize(mNormal * vec4(inNormal, 0));

    vec4 position = TransformFromModelToView(vec4(inVertex, 1));

    if (lightPosition.w == 0)
        light.xyz = normalize(TransformFromWorldToView(lightPosition).xyz);

    else light.xyz = normalize(TransformFromWorldToView(lightPosition).xyz - position.xyz);

    gl_Position = TransformFromViewToClip(position);
}

layout(index = 1) subroutine(RenderPassType)
void fullQuadRender()
{
    gl_Position = vec4(inVertex.xy, 1.0, 1.0);
    texCoord = inVertex.xy * 0.5 + 0.5;

    ray = (viewport.invProj * gl_Position).xyz;
}

void main()
{
    RenderPass();
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

subroutine void RenderPassType();
layout(location = 0) subroutine uniform RenderPassType RenderPass;

layout(location = nBASE_COLOR) out vec4 fragColor;
layout(location = nNORMALS) out vec2 fragNormal;

layout(binding = nALBEDO) uniform sampler2D colorSampler;
//layout(bindless_sampler) uniform sampler2D colorSampler;
layout(binding = nNORMAL_MAP) uniform sampler2D normalSampler;
//layout(bindless_sampler) uniform sampler2DMS normalSampler;
//layout(binding = nDEPTH) uniform sampler2D depthSampler;
layout(bindless_sampler) uniform sampler2D depthSampler;
layout(binding = 4) uniform sampler2D noiseSampler;

layout(location = 64) uniform vec3 samples[64];

struct PointLight {
    vec3 position;
    vec3 color;
};

const int kPOINT_LIGHTS = 5;
layout(location = 10) uniform PointLight pointLights[kPOINT_LIGHTS] = {
    { { -6, 1.4, 1.6 }, { 0, 1, 0 } },
    { { 2, 0, 2 }, { 0, 0.8, 1 } },
    { { 11.2, 1.2, -4.5 }, { 1, 0.24, 0 } },
    { { 5, 1, -2 }, { 1, 0.8, 0.4 } },
    { { -6, 1.4, -1.6 }, { 1, 1, 0 } }
};

const vec2 noiseScale = vec2(1920.0 / 4.0, 1080.0 / 4.0);
const float radius = 0.25;
const float bias = 0.0125;
const float kernelSize = 64;

in vec4 light;
in vec4 normal;
in vec2 texCoord;

in vec3 ray;

#define HALF_LAMBERT 1
#define WRAPPED_AROUND_LAMBERT 0

layout(index = 0) subroutine(RenderPassType)
void renderGBuffer()
{
    vec3 n = normalize(normal.xyz);
    vec3 l = normalize(light.xyz);

#if HALF_LAMBERT
    float diffuse = max(dot(n, l) * 0.5 + 0.5, 0);
#elif WRAPPED_AROUND_LAMBERT
    const float offset = 0.5;
    float diffuse = max(dot(n, l) + offset, 0) / (1 + offset);
#else
    float diffuse = max(dot(n, l), 0);
#endif

    fragColor = vec4(vec3(diffuse), 1);
    fragNormal = EncodeNormal(n);
}

layout(index = 1) subroutine(RenderPassType)
void ssao()
{
    fragColor = texture(colorSampler, texCoord);

    float depth = texture(depthSampler, texCoord).x;
    depth = HyperbolicDepthToLinear(depth);

    vec3 p = ray * depth;
    //p.z *= -1;

#if 0
    fragColor = vec4(vec3(0.16), 0);

    vec3 lightPos;
    float dist, attenuation;

    for (int i = 0; i < kPOINT_LIGHTS; ++i) {
        lightPos = TransformFromWorldToView(vec4(pointLights[i].position, 1)).xyz;

        dist = distance(p, lightPos);
        attenuation = 1.0 / (1.0 + dist * (0.7 + dist * 1.8));

        fragColor.rgb += pointLights[i].color * attenuation;
    }
#endif

    vec3 n = DecodeNormal(texelFetch(normalSampler, ivec2(gl_FragCoord), gl_SampleID).xy);
    //vec3 n = DecodeNormal(texture(normalSampler, texCoord).xy);
    vec3 rvec = texture(noiseSampler, texCoord * noiseScale).rgb;

    vec3 t = normalize(rvec - n * dot(rvec, n));
    vec3 b = cross(n, t);
    mat3 TBN = mat3(t, b, n);

    float occlusion = 0, rejectedSamples = 0;
    vec3 s, sdir;
    vec4 o;

    for (int i = 0; i < kernelSize; ++i) {
        s = TBN * samples[i];
        s = p + s * radius;// / (depth);

        sdir = normalize(s - p);
        float nDotS = max(dot(n, sdir), 0);

        o = viewport.proj * vec4(s, 1);
        o.xy /= o.w;
        o.xy = fma(o.xy, vec2(0.5), vec2(0.5));

        float d = texture(depthSampler, o.xy).x;
        d = HyperbolicDepthToLinear(d);

        float rangeCheck = smoothstep(0, 1, radius / (depth - d));
        // rangeCheck = 1.0 - pow(abs(d - depth) / radius, 2.0);

        occlusion += step(s.z + bias, -d) * nDotS * rangeCheck;
    }

    occlusion = max(1 - (occlusion / (kernelSize - rejectedSamples)), 0);

    fragColor.rgb *= pow(occlusion, 1);
}


const vec2 kInvResolution = 1.0 / vec2(1920.0, 1080.0);

layout(binding = 5, std430) readonly buffer GAUSS_FILTER_COLOR_WEIGHTS
{
    float weights[kKERNEL_SIZE];
};

#if GPU_FILTERED_GAUSSIAN_BLUR && !BILATERAL_GAUSSIAN_GILTER
layout(binding = 6, std430) readonly buffer GAUSS_FILTER_OFFSETS
{
    float offsets[kKERNEL_SIZE];
};
#endif

#if BILATERAL_GAUSSIAN_GILTER
const float kSharpness = 40.0;

float GetDepthBasedRangeWeight(in float center_depth, in ivec2 center, in ivec2 offset)
{
    float depth = texelFetchOffset(depthSampler, center, 0, offset).x;
    depth = HyperbolicDepthToLinear(depth);

    float depth_diff = (depth - center_depth) * kSharpness;
    float w = exp2(depth_diff * depth_diff);

    return w;
}
#endif

vec4 blur_pass(in vec2 direction)
{
#if BILATERAL_GAUSSIAN_GILTER
    ivec2 center = ivec2(gl_FragCoord.xy), offset = ivec2(direction);

    vec4 sum = texelFetch(colorSampler, center, 0) * weights[0];

    float center_depth = texelFetch(depthSampler, center, 0).x;
    center_depth = HyperbolicDepthToLinear(center_depth);

    float total_weight = 0.0;

    for (int i = 1; i < kKERNEL_SIZE; ++i) {
        float weight = weights[i] / GetDepthBasedRangeWeight(center_depth, center, offset * i);

        total_weight += weight;

        sum += texelFetchOffset(colorSampler, center, 0, +offset * i) * weight;
        sum += texelFetchOffset(colorSampler, center, 0, -offset * i) * weight;
    }

    return sum / (fma(2.0, total_weight, weights[0]));

#elif GPU_FILTERED_GAUSSIAN_BLUR
    vec4 sum = texture(colorSampler, gl_FragCoord.xy * kInvResolution) * weights[0];

    for (int i = 1; i < kKERNEL_SIZE; ++i) {
        sum += texture(colorSampler, (gl_FragCoord.xy + direction * offsets[i]) * kInvResolution) * weights[i];
        sum += texture(colorSampler, (gl_FragCoord.xy - direction * offsets[i]) * kInvResolution) * weights[i];
    }

#else
    ivec2 center = ivec2(gl_FragCoord.xy), offset = ivec2(direction);

    vec4 sum = texelFetch(colorSampler, center, 0) * weights[0];

    for (int i = 1; i < kKERNEL_SIZE; ++i) {
        sum += texelFetchOffset(colorSampler, center, 0, +offset * i) * weights[i];
        sum += texelFetchOffset(colorSampler, center, 0, -offset * i) * weights[i];
    }

#endif

    return sum;
}

layout(index = 3) subroutine(RenderPassType)
void blur_pass_vertical()
{
    fragColor.rgb = blur_pass(vec2(0, 1)).rgb;
}

layout(index = 4) subroutine(RenderPassType)
void blur_pass_horizontal()
{
    fragColor.rgb = blur_pass(vec2(1, 0)).rgb;
}

void main()
{
    RenderPass();
}