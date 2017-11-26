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



#	pragma stage ("vertex")

layout(location = nPOSITION) in vec3 inVertex;
layout(location = nCOLOR) in vec3 inColor;

out vec3 color;

void main()
{
    gl_Position = TransformFromWorldToClip(vec4(inVertex, 1));
    color = inColor;
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

layout(location = nBASE_COLOR/*, index = 0*/) out vec4 fragColor;

in vec3 color;

void main()
{
    fragColor = vec4(color, 1.0);
}
