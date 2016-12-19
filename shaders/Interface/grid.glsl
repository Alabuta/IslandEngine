/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Started at 31th October 2010.
****    Description: grid render shader file.
****
********************************************************************************************************************************/
#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;

layout(binding = nTRANSFORM, row_major) uniform TRSM
{
    mat4 mModel;
    mat4 mNormalMatrix; // Inverted and transposed model (world) matrix.

    mat4 mViewProj;

    mat4 mView;
    mat4 mProj;
};

void main()
{
    gl_Position = mViewProj * vec4(inVertex, 1.0);
}

#elif CRUS_FRAGMENT_SHADER

layout(binding = nMATERIAL, shared) uniform CMTS
{
    vec4 Clr0;
    vec4 Clr1;
    vec4 Clr2;
    vec4 Clr3;
};

layout(location = nFRAG_COLOR, index = 0) out vec4 FragColor;
uniform vec4 LineColor;

void main()
{
    FragColor = LineColor;
}

#endif