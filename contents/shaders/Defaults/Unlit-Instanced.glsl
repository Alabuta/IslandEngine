/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: default instancing shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;

void main()
{
    gl_Position = TransformInstancedFromModelToClip(inVertex);
}

#elif CRUS_FRAGMENT_SHADER

layout(early_fragment_tests) in;

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0);
}

#endif