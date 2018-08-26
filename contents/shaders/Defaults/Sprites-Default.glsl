/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: default sprites shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#pragma stage("vertex")

layout(location = nPOSITION) in vec3 inVertex;
layout(location = nNORMAL) in vec3 inNormal;
layout(location = nTEX_COORD) in vec2 inTexCoord;

out vec2 texCoord;

void main()
{
    gl_Position = TransformFromModelToClip(inVertex);

    texCoord = inTexCoord;
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

layout(location = nBASE_COLOR/*, index = 0*/) out vec4 fragColor;
/*layout(location = nMAIN_COLOR)*/ uniform vec4 mainColor = vec4(1.0);

layout(binding = 0) uniform sampler2D mainTexture;

in vec2 texCoord;

noperspective in vec3 edge_distance;

void main()
{
    fragColor = texture(mainTexture, texCoord) * mainColor;
}
