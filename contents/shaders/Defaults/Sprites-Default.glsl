/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: default sprites shader file.
****
********************************************************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;
layout(location = nTEXCRD) in vec2 inTexCoord;

out vec2 texCoord;

void main()
{
    gl_Position = TransformFromWorldToClip(inVertex);

    texCoord = inTexCoord;
}

#elif CRUS_FRAGMENT_SHADER

layout(location = nFRAG_COLOR, index = 0) out vec4 FragColor;

uniform sampler2D mainTexture;
layout(location = nMAIN_COLOR) uniform vec4 mainColor = vec4(1.0);

in vec2 texCoord;

void main()
{
    FragColor = texture2D(mainTexture, texCoord) * mainColor;
}

#endif