/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: built-in shader variables header file.
****
********************************************************************************************************************************/

#if CRUS_VERTEX_SHADER || CRUS_GEOMETRY_SHADER

layout(binding = nVIEWPORT, std140) uniform VIEWPORT
{
    layout(row_major) mat4 mProjView;

    layout(row_major) mat4 mProj;
    layout(row_major) mat4 mView;
    layout(row_major) mat4 mViewport;
};

layout(binding = nTRANSFORM, std430) buffer TRANSFORM
{
    layout(row_major) readonly mat4 mProjViewModel;

    layout(row_major) readonly mat4 mModel;
    layout(row_major) readonly mat4 mNormal; // Inverted and transposed model (world) matrix.
};

#elif CRUS_FRAGMENT_SHADER

#endif // CRUS_VERTEX_SHADER & CRUS_FRAGMENT_SHADER