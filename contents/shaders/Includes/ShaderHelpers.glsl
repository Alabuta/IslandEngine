/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: shader helpers header file.
****
********************************************************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")

#ifndef CRUS_SHADER_HELPERS
#define CRUS_SHADER_HELPERS

#if CRUS_VERTEX_SHADER

vec4 TransformFromWorldToClip(in vec3 position)
{
    return mProjViewModel * vec4(position, 1.0);
}

#elif CRUS_FRAGMENT_SHADER

#endif // CRUS_VERTEX_SHADER & CRUS_FRAGMENT_SHADER

#endif // CRUS_SHADER_HELPERS