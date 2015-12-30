/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 3rd July 2011.
****    Description: uniform buffer objects initialization shader.
****
****************************************************************************************/

#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;

layout(binding = nTRANSFORM, row_major) uniform TRSM {
    mat4 mModel;
    mat4 mNormalMatrix; // Inverted and transposed model (world) matrix.

    mat4 mViewProj;

    mat4 mView;
    mat4 mProj;
};

/*layout(binding = nATLAS, shared) uniform ATLAS {
    vec2 inAtlasCoord;
    vec2 inAtlasSize;
};*/

void main()
{
    gl_Position = mViewProj * mModel * vec4(1.0);
}

#elif CRUS_FRAGMENT_SHADER

layout(binding = nMATERIAL, shared) uniform CMTS {
    vec4 Clr0;
    vec4 Clr1;
    vec4 Clr2;
    vec4 Clr3;
};

layout(location = nFRAG_COLOR, index = 0) out vec4 FragColor;

void main()
{
    FragColor = Clr0 * vec4(0.4);
}

#endif