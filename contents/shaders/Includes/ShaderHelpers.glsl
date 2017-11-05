/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: shader helpers header file.
****
********************************************************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")

#if CRUS_VERTEX_STAGE

vec2 clippedToViewport(in vec4 position)
{
    return vec2(viewport.z * 0.5 * (position.x / position.w + 1.0) + viewport.x,
                viewport.w * 0.5 * (position.y / position.w + 1.0) + viewport.y);
}

vec2 normalizedToViewport(in vec2 position)
{
    return vec2(viewport.z * 0.5 * (position.x + 1.0) + viewport.x,
                viewport.w * 0.5 * (position.y + 1.0) + viewport.y);
}

vec4 TransformFromModelToClip(vec4 position)
{
    return mProjViewModel * position;
}

vec4 TransformFromWorldToClip(vec4 position)
{
    return mProjView * position;
}

vec4 TransformFromViewToClip(vec4 position)
{
    return mProj * position;
}

vec4 TransformFromWorldToView(vec4 position)
{
    return mView * position;
}

vec4 TransformFromModelToView(vec4 position)
{
    return mView * mModel * position;
}

vec4 TransformFromModelToWorld(vec4 position)
{
    return mModel * position;
}

#elif CRUS_FRAGMENT_STAGE

#endif // CRUS_VERTEX_STAGE & CRUS_FRAGMENT_STAGE