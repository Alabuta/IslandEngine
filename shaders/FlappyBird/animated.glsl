/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    27th March 2014 / 28th October 2010.
****    Description: animated sprite shader file.
****
****************************************************************************************/
#extension GL_ARB_shading_language_420pack : enable
#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;
layout(location = nTEXCRD) in vec2 inTexCoord;

uniform vec2 inAnimTexCoord;
uniform vec2 inAtlasCoord;
uniform vec2 inAtlasSize;

uniform vec4 inDispCoord;

out vec2 texCoord;

layout(binding = nTRANSFORM, row_major) uniform TRSM {
    mat4 mModel;
    mat4 mNormalMatrix; // Inverted and transposed model (world) matrix.

    mat4 mViewProj;

    mat4 mView;
    mat4 mProj;
};

void main()
{
    gl_Position = mViewProj * mModel * vec4(inVertex, 1.0);

    texCoord = inDispCoord.xy + inTexCoord + inDispCoord.zw;
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