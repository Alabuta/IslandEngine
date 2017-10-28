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

vec4 TransformFromModelToClip(vec3 position)
{
    return mProjViewModel * vec4(position, 1.0);
}

vec4 TransformFromWorldToClip(vec3 position)
{
    return mProjView * vec4(position, 1.0);
}

#elif CRUS_FRAGMENT_STAGE

#endif // CRUS_VERTEX_STAGE & CRUS_FRAGMENT_STAGE