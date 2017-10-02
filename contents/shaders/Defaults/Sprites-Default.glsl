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

#if CRUS_VERTEX_SHADER
#pragma shader_stage("vertex")

layout(location = nVERTEX) in vec3 inVertex;
layout(location = nNORMAL) in vec3 inNormal;
layout(location = nTEXCRD) in vec2 inTexCoord;

out vec2 texCoord;

void main()
{
    gl_Position = TransformFromModelToClip(inVertex);

    texCoord = inTexCoord;
}

#elif CRUS_FRAGMENT_SHADER
#pragma shader_stage("fragment")

layout(early_fragment_tests) in;

layout(location = nFRAG_COLOR/*, index = 0*/) out vec4 FragColor;
layout(location = nMAIN_COLOR) uniform vec4 mainColor = vec4(1.0);

layout(binding = 0) uniform sampler2D mainTexture;

in vec2 texCoord;

noperspective in vec3 edge_distance;

void main()
{
    FragColor = texture2D(mainTexture, texCoord) * mainColor;
}

#endif