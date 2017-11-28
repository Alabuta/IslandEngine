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

/*vec2 clippedToViewport(in vec4 position)
{
    return vec2(viewport.rect.z * 0.5 * (position.x / position.w + 1.0) + viewport.rect.x,
                viewport.rect.w * 0.5 * (position.y / position.w + 1.0) + viewport.rect.y);
}

vec2 normalizedToViewport(in vec2 position)
{
    return vec2(viewport.rect.z * 0.5 * (position.x + 1.0) + viewport.rect.x,
                viewport.rect.w * 0.5 * (position.y + 1.0) + viewport.rect.y);
}*/

float HyperbolicDepthToLinear(float hyperbolicDepth)
{
#if CRUS_DEPTH_CLIPPED_FROM_ZERO_TO_ONE
    #if CRUS_REVERSED_DEPTH
        #if CRUS_INFINITE_FAR_PLANE
            return zNear / hyperbolicDepth;
        #else
            return zNear * zFar / (hyperbolicDepth * (zNear - zFar) + zNear);
        #endif
    #else
        return -zNear * zFar / (hyperbolicDepth * (zNear - zFar) + zFar);
    #endif
#else
    return -2.0 * zNear * zFar / (hyperbolicDepth * (zNear - zFar) + zFar + zNear);
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

