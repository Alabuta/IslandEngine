/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: shader helpers header file.
****
********************************************************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")

const float zNear = 0.01, zFar = 100.0;

vec2 clippedToViewport(in vec4 position)
{
    const vec4 rect = vec4(0, 0, 1920, 1080);
    return vec2(rect.z * 0.5 * (position.x / position.w + 1.0) + rect.x,
                rect.w * 0.5 * (position.y / position.w + 1.0) + rect.y);
}

vec2 normalizedToViewport(in vec2 position)
{
    const vec4 rect = vec4(0, 0, 1920, 1080);
    return vec2(rect.z * 0.5 * (position.x + 1.0) + rect.x,
                rect.w * 0.5 * (position.y + 1.0) + rect.y);
}

float HyperbolicDepthToLinear(float hyperbolicDepth)
{
#if CRUS_DEPTH_CLIPPED_FROM_ZERO_TO_ONE
    #if CRUS_REVERSED_DEPTH
        #if CRUS_INFINITE_FAR_PLANE
            return zNear / hyperbolicDepth;
        #else
            return -zNear * zFar / (hyperbolicDepth * (zNear - zFar) + zNear);
        #endif
    #else
        return zNear * zFar / (hyperbolicDepth * (zNear - zFar) + zFar);
    #endif
#else
    return -2.0 * zNear * zFar / (hyperbolicDepth * (zFar - zNear) - (zFar + zNear));
#endif
}

vec4 TransformFromModelToClip(vec4 position)
{
    return mProjViewModel * position;
}

vec4 TransformFromWorldToClip(vec4 position)
{
    return viewport.projView * position;
}

vec4 TransformFromViewToClip(vec4 position)
{
    return viewport.proj * position;
}

vec4 TransformFromWorldToView(vec4 position)
{
    return viewport.view * position;
}

vec4 TransformFromModelToView(vec4 position)
{
    return viewport.view * mModel * position;
}

vec4 TransformFromModelToWorld(vec4 position)
{
    return mModel * position;
}

// Lambert Azimuthal Equal-Area projection
vec2 EncodeNormal(vec3 n)
{
    float f = sqrt(fma(8, n.z, 8));
    return n.xy / f + 0.5;
}

vec3 DecodeNormal(vec2 enc)
{
    vec2 fenc = fma(enc, vec2(4), vec2(-2));
    float f = dot(fenc, fenc);
    float g = sqrt(1 - f / 4.0);

    vec3 n = vec3(fenc * g, 1 - f / 2.0);
    return n;
}