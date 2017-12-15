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

out vec3 pos;
out vec3 ray;

layout(index = 0) subroutine (RenderPassType)
void modelProccessing()
{
    texCoord = inTexCoord;

    normal = normalize(mNormal * vec4(inNormal, 0));

    vec4 position = TransformFromModelToView(vec4(inVertex, 1));

    if (lightPosition.w == 0)
        light.xyz = normalize(TransformFromWorldToView(lightPosition).xyz);

    else light.xyz = normalize(TransformFromWorldToView(lightPosition).xyz - position.xyz);

    gl_Position = TransformFromViewToClip(position);

    pos = position.xyz;
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
layout(location = 2) out float fragTemp;

layout(binding = nALBEDO) uniform sampler2D colorSampler;
layout(binding = nNORMAL_MAP) uniform sampler2D normalSampler;
layout(binding = nDEPTH) uniform sampler2D depthSampler;
layout(binding = 4) uniform sampler2D noiseSampler;

layout(location = 8) uniform vec3 samples[64];

const vec2 noiseScale = vec2(1920.0 / 4.0, 1080.0 / 4.0);
const float radius = 0.25;
const float bias = 0.0125;
const float kernelSize = 64;

in vec4 light;
in vec4 normal;
in vec2 texCoord;

in vec3 pos;
in vec3 ray;

#define HALF_LAMBERT 1
#define WRAPPED_AROUND_LAMBERT 0

layout(index = 0) subroutine(RenderPassType)
void renderGBuffer()
{
    vec3 n = normalize(normal.xyz);
    vec3 l = normalize(light.xyz);

#if HALF_LAMBERT
    float diffuse = (dot(n, l) * 0.5 + 0.5);
#elif WRAPPED_AROUND_LAMBERT
    const float offset = 0.5;
    float diffuse = max(dot(n, l) + offset, 0) / (1 + offset);
#else
    float diffuse = dot(n, l);
#endif

    fragColor = vec4(vec3(1), 1);
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

    vec3 n = DecodeNormal(texture(normalSampler, texCoord).xy);
    vec3 rvec = texture(noiseSampler, texCoord * noiseScale).rgb;

    vec3 t = normalize(rvec - n * dot(rvec, n));
    vec3 b = cross(n, t);
    mat3 TBN = mat3(t, b, n);

    float occlusion = 0, rejectedSamples = 0;
    vec3 s, sdir;
    vec4 o;

    for (int i = 0; i < kernelSize; ++i) {
        s = TBN * samples[i];
        s = p + s * radius;// * max(1, -depth);

        sdir = normalize(s - p);
        float nDotS = max(dot(n, sdir), 0);

        o = viewport.proj * vec4(s, 1);
        o.xy /= o.w;
        o.xy = fma(o.xy, vec2(0.5), vec2(0.5));

        float d = texture(depthSampler, o.xy).x;
        d = HyperbolicDepthToLinear(d);

        float rangeCheck = smoothstep(0, 1, radius / abs(d - depth));
#if 0
        float tttt = step(s.z + bias, -d);

        if (abs(d - depth) > radius)
            rejectedSamples += 1;

        occlusion += tttt * rangeCheck * nDotS;
#else
        occlusion += step(s.z + bias, -d) * rangeCheck * nDotS;
#endif
    }

    occlusion = max(0, 1 - (occlusion / (kernelSize - rejectedSamples)));

    fragColor.rgb *= pow(occlusion, 1);
}

layout(index = 2) subroutine(RenderPassType)
void blur()
{
    vec2 texelSize = 1.0 / vec2(textureSize(colorSampler, 0));
    float result = 0;

    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(colorSampler, texCoord + offset).x;
        }
    }

    fragColor.rgb = vec3(result / (4.0 * 4.0));
    //fragColor = texture(colorSampler, texCoord);

    fragColor.a = texture(colorSampler, gl_FragCoord.xy).a;
}

const float KERNEL_RADIUS = 3.0;
const float g_Sharpness = 40.0;
const vec2  g_InvResolutionDirection = 1.0 / vec2(1920.0, 1080.0);

vec4 BlurFunction(vec2 uv, float r, vec4 center_c, float center_d, inout float w_total)
{
    vec4  c = texture(colorSampler, uv);
    float d = texture(depthSampler, uv).x;
    d = HyperbolicDepthToLinear(d);

    const float BlurSigma = float(KERNEL_RADIUS) * 0.5;
    const float BlurFalloff = 1.0 / (2.0 * BlurSigma * BlurSigma);

    float ddiff = (d - center_d) * g_Sharpness;
    float w = exp2(-r * r * BlurFalloff - ddiff * ddiff);
    w_total += w;

    return c * w;
}

#if 1
uniform float Weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

layout(index = 3) subroutine(RenderPassType)
void blur_pass_vertical()
{
    vec2 res = 1.0 / vec2(1920.0, 1080.0);

    vec4  center_c = texture(colorSampler, texCoord);
    float center_d = texture(depthSampler, texCoord).x;
    center_d = HyperbolicDepthToLinear(center_d);

    vec4 c_total = center_c;
    float w_total = 1.0;

    vec2 uv;

    for (float r = 1; r <= KERNEL_RADIUS; ++r) {
        uv = fma(res, vec2(0, +r), texCoord);
        c_total += BlurFunction(uv, r, center_c, center_d, w_total);
    }

    for (float r = 1; r <= KERNEL_RADIUS; ++r) {
        uv = fma(res, vec2(0, -r), texCoord);
        c_total += BlurFunction(uv, r, center_c, center_d, w_total);
    }

    fragColor = c_total / w_total;

    /*ivec2 pix = ivec2(gl_FragCoord.xy);

    vec4 sum = texelFetch(colorSampler, pix, 0) * Weight[0];

    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(0,  1)) * Weight[1];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(0, -1)) * Weight[1];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(0,  2)) * Weight[2];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(0, -2)) * Weight[2];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(0,  3)) * Weight[3];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(0, -3)) * Weight[3];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(0,  4)) * Weight[4];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(0, -4)) * Weight[4];

    fragColor = sum;*/
}

layout(index = 4) subroutine(RenderPassType)
void blur_pass_horizontal()
{
    /*ivec2 pix = ivec2(gl_FragCoord.xy);

    vec4 sum = texelFetch(colorSampler, pix, 0) * Weight[0];

    sum += texelFetchOffset(colorSampler, pix, 0, ivec2( 1, 0)) * Weight[1];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(-1, 0)) * Weight[1];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2( 2, 0)) * Weight[2];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(-2, 0)) * Weight[2];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2( 3, 0)) * Weight[3];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(-3, 0)) * Weight[3];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2( 4, 0)) * Weight[4];
    sum += texelFetchOffset(colorSampler, pix, 0, ivec2(-4, 0)) * Weight[4];

    fragColor = sum;*/

    vec2 res = 1.0 / vec2(1920.0, 1080.0);

    vec4  center_c = texture(colorSampler, texCoord);
    float center_d = texture(depthSampler, texCoord).x;
    center_d = HyperbolicDepthToLinear(center_d);

    vec4 c_total = center_c;
    float w_total = 1.0;

    vec2 uv;

    for (float r = 1; r <= KERNEL_RADIUS; ++r) {
        uv = fma(res, vec2(+r, 0), texCoord);
        c_total += BlurFunction(uv, r, center_c, center_d, w_total);
    }

    for (float r = 1; r <= KERNEL_RADIUS; ++r) {
        uv = fma(res, vec2(-r, 0), texCoord);
        c_total += BlurFunction(uv, r, center_c, center_d, w_total);
    }

    fragColor = c_total / w_total;
}
#else

uniform float offsets[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weights[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

layout(index = 3) subroutine(RenderPassType)
void blur_pass_vertical()
{
    vec2 res = 1.0 / vec2(1920.0, 1080.0);
    vec4 sum = texture(colorSampler, gl_FragCoord.xy * res) * weights[0];

    sum += texture(colorSampler, (gl_FragCoord.xy + vec2(0, offsets[1])) * res) * weights[1];
    sum += texture(colorSampler, (gl_FragCoord.xy - vec2(0, offsets[1])) * res) * weights[1];
    sum += texture(colorSampler, (gl_FragCoord.xy + vec2(0, offsets[2])) * res) * weights[2];
    sum += texture(colorSampler, (gl_FragCoord.xy - vec2(0, offsets[2])) * res) * weights[2];

    fragColor = sum;
}

layout(index = 4) subroutine(RenderPassType)
void blur_pass_horizontal()
{
    vec2 res = 1.0 / vec2(1920.0, 1080.0);
    vec4 sum = texture(colorSampler, gl_FragCoord.xy * res) * weights[0];

    sum += texture(colorSampler, (gl_FragCoord.xy + vec2(offsets[1], 0)) * res) * weights[1];
    sum += texture(colorSampler, (gl_FragCoord.xy - vec2(offsets[1], 0)) * res) * weights[1];
    sum += texture(colorSampler, (gl_FragCoord.xy + vec2(offsets[2], 0)) * res) * weights[2];
    sum += texture(colorSampler, (gl_FragCoord.xy - vec2(offsets[2], 0)) * res) * weights[2];

    fragColor = sum;
}
#endif

void main()
{
    RenderPass();
}