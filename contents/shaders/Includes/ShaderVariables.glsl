/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: built-in shader variables header file.
****
********************************************************************************************************************************/



//layout(location = nVIEWPORT_RECT) uniform vec4 viewport = vec4(0, 0, 1920, 1080);
//layout(location = nNEAR_FAR) uniform vec2 near_far = vec2(0.01, 100.0);

layout(binding = nVIEWPORT, std140) uniform VIEWPORT
{
    layout(row_major) mat4 projView;

    layout(row_major) mat4 proj;
    layout(row_major) mat4 view;

    layout(row_major) mat4 invProj;

    //vec4 rect = vec4(0, 0, 1920, 1080);
    //vec2 clipPlanes;
} viewport;


layout(binding = nTRANSFORM, std430) readonly buffer TRANSFORM
{
    layout(row_major) readonly mat4 mProjViewModel;

    layout(row_major) readonly mat4 mModel;
    layout(row_major) readonly mat4 mNormal; // Transpose of the inversed of the upper left 3x3 sub-matrix of model(world)-view matrix.
};


layout(binding = 2, std430) readonly buffer PER_OBJECT
{
	mat4 world;
	mat4 normal;  // Transposed of the inversed of the upper left 3x3 sub-matrix of world matrix.
} object;

layout(binding = 3, std430) readonly buffer PER_CAMERA
{
	mat4 view;
	mat4 projection;

	mat4 projectionView;

	mat4 invertedView;
	mat4 invertedProjection;
} camera;
