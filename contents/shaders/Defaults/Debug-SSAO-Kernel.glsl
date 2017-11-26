/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: default unlit shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#pragma stage("vertex")

layout(location = nPOSITION) in vec3 inVertex;

out vec4 origin;

void main()
{
    origin = TransformFromModelToClip(vec4(0, 0, 0, 1));
    gl_Position = TransformFromModelToClip(vec4(inVertex, 1));
}

#pragma stage("geometry")

in vec4 origin[];

layout(points) in;
layout(line_strip, max_vertices = 2) out;

void main()
{
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = origin[0];
    EmitVertex();

    EndPrimitive();
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

layout(location = nBASE_COLOR) out vec4 fragColor;

void main()
{
    fragColor = vec4(1, 0, 0, 1);
}
