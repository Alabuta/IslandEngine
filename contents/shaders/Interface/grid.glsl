/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: grid render shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;
layout(location = nCOLOR) in vec3 inColor;

out vec3 color;

void main()
{
    gl_Position = TransformFromWorldToClip(inVertex);
    color = inColor;
}

#elif CRUS_FRAGMENT_SHADER

layout(early_fragment_tests) in;

layout(location = nFRAG_COLOR/*, index = 0*/) out vec4 FragColor;

in vec3 color;

void main()
{
    FragColor = vec4(color, 1.0);
}

#endif