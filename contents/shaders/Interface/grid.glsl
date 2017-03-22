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

void main()
{
    gl_Position = TransformFromWorldToClip(inVertex);
}

#elif CRUS_FRAGMENT_SHADER

layout(location = nFRAG_COLOR, index = 0) out vec4 FragColor;
layout(location = nMAIN_COLOR) uniform vec4 mainColor = vec4(vec3(0.0), 1.0);

void main()
{
    FragColor = mainColor;
}

#endif