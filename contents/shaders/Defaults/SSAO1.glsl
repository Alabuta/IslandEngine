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

    ray = (inverse(viewport.proj) * gl_Position).xyz;
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
layout(binding = 2) uniform sampler2D tempSampler;

layout(location = 8) uniform vec3 samples[64];

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

    fragColor = vec4(vec3(diffuse), 1.0);
    fragNormal = n.xy;
}

layout(index = 1) subroutine(RenderPassType)
void ssao()
{
    fragColor.rgb = texture(colorSampler, texCoord).rgb;
    fragColor.a = 1;
}

layout(index = 2) subroutine(RenderPassType)
void blur()
{
    fragColor.rgb = texture(colorSampler, texCoord).rgb;

    vec3 n = vec3(texture(normalSampler, texCoord).xy, 0);
    n.z = sqrt(fma(-n.y, n.y, fma(n.x, -n.x, 1)));

    float d = texture(depthSampler, texCoord).x;
    d = HyperbolicDepthToLinear(d);

    vec3 POS = ray * d;
    POS.z *= -1;

    //fragColor.rgb = POS * 0.5 + 0.5;

    fragColor.a = 1;
}

void main()
{
    RenderPass();
}