/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: built-in shader variables header file.
****
********************************************************************************************************************************/


#if CRUS_VERTEX_STAGE || CRUS_GEOMETRY_STAGE

layout(location = nVIEWPORT_RECT) uniform vec4 viewport = vec4(0, 0, 1920, 1080);

layout(binding = nVIEWPORT, std140) uniform VIEWPORT
{
    layout(row_major) mat4 mProjView;

    layout(row_major) mat4 mProj;
    layout(row_major) mat4 mView;
    layout(row_major) mat4 mViewport;
};

layout(binding = nTRANSFORM, std430) readonly buffer TRANSFORM
{
    layout(row_major) readonly mat4 mProjViewModel;

    layout(row_major) readonly mat4 mModel;
    layout(row_major) readonly mat4 mNormal; // It's the transpose of the inverse of the upper left 3x3 sub-matrix of model(world)-view matrix.
};

#elif CRUS_FRAGMENT_STAGE

#endif // CRUS_VERTEX_STAGE & CRUS_FRAGMENT_STAGE