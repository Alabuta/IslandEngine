/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 10th December 2015.
****    Description: background shader file.
****
********************************************************************************************************************************/

#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;
layout(location = nTEXCRD) in vec2 inTexCoord;
out vec2 texCoord;

void main()
{
    gl_Position = vec4(inVertex, 1.0);

    texCoord = inTexCoord;//inVertex.xy * 0.5 + 0.5;
}

#elif CRUS_FRAGMENT_SHADER

layout(location = nFRAG_COLOR, index = 0) out vec4 FragColor;
uniform sampler2D albedoTexture;
in vec2 texCoord;

void main()
{
    FragColor = texture(albedoTexture, texCoord);
}

#endif