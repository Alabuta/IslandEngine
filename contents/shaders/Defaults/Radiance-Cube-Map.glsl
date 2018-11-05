/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: default cube map skybox shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#pragma stage("vertex")

layout(location = nPOSITION) in vec3 inVertex;

out vec3 texCoord;

subroutine void RenderPassType();
layout(location = 0) subroutine uniform RenderPassType RenderPass;


layout(index = 0) subroutine(RenderPassType)
void equirectangularMapRender()
{
    gl_Position = TransformFromWorldToClip(vec4(inVertex, 0));

    texCoord = inVertex;
}

layout(index = 1) subroutine(RenderPassType)
void cubeMapRender()
{
    gl_Position = TransformFromWorldToClip(vec4(inVertex, 0));

    // You can just assign w component to z,
    // but then you have to change depth comprasion function to GL_LEQUAL(GL_GEQUAL).
    // Instead, consider code example below.
#if CRUS_REVERSED_DEPTH
    gl_Position.z = gl_Position.w * 0.000001;
#else
    gl_Position.z = gl_Position.w * 0.999999;
#endif

    texCoord = -inVertex;
}

void main()
{
    RenderPass();
}

#pragma stage("fragment")

subroutine void RenderPassType();
layout(location = 0) subroutine uniform RenderPassType RenderPass;

layout(early_fragment_tests) in;

layout(location = nBASE_COLOR) out vec4 fragColor;

layout(bindless_sampler, location = nALBEDO) uniform sampler2D equirectangularMap;
layout(binding = nNORMAL_MAP) uniform samplerCube cubeMap;
//layout(bindless_sampler, location = nNORMAL_MAP) uniform samplerCube cubeMap;


in vec3 texCoord;


const vec2 invAtan = vec2(0.1591, 0.3181);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));

    return uv * invAtan + 0.5;
}

layout(index = 0) subroutine(RenderPassType)
void equirectangularMapRender()
{
    vec2 uv = SampleSphericalMap(normalize(texCoord));
    vec3 envColor = texture(equirectangularMap, uv).rgb;

    envColor = envColor / (envColor + vec3(1));
    envColor = pow(envColor, vec3(1.0 / 2.2));

    fragColor = vec4(envColor, 1);
}

layout(index = 1) subroutine(RenderPassType)
void cubeMapRender()
{
    fragColor = texture(cubeMap, normalize(texCoord));
}

void main()
{
    RenderPass();
}
