/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: test shader file.
****
********************************************************************************************************************************/
//#extension GL_ARB_conservative_depth : enable

//const float Fcoef = 2.0 / log2(4194304.0 + 1.0);

#if CRUS_VERTEX_STAGE

layout(location = nPOSITION) in vec3 inVertex;
layout(location = nTEX_COORD) in vec2 inTexCoord;

out vec2 texCoord;

layout(binding = nVIEWPORT, row_major) uniform VIEWPORT
{
    mat4 mModel;
    mat4 mNormalMatrix; // Inverted and transposed model (world) matrix.

    mat4 mViewProj;

    mat4 viewport.view;
    mat4 viewport.proj;
};

void main()
{
    gl_Position = mViewProj * mModel * vec4(inVertex, 1.0);

    texCoord = inTexCoord;
}

#elif CRUS_FRAGMENT_STAGE

layout(early_fragment_tests) in;

layout(binding = nMATERIAL, shared) uniform CMTS
{
    vec4 Clr0;
    vec4 Clr1;
    vec4 Clr2;
    vec4 Clr3;
};

layout(location = nBASE_COLOR/*, index = 0*/) out vec4 fragColor;
//layout(depth_less) out float gl_FragDepth;

//in float logz;

uniform sampler2D albedoTexture;
in vec2 texCoord;

void main()
{
    //fragColor = vec4(pow(Clr0.rgb, vec3(1.0 / 2.2)), Clr0.a);
    //fragColor = vec4(sqrt(Clr0.rgb), Clr0.a);
    //fragColor = Clr0;
    fragColor = texture2D(albedoTexture, texCoord);
    //fragColor = vec4(vec3(1, 0, 0), 1.0);

    //gl_FragDepth = log2(logz) * 0.5 * Fcoef;
}

#endif
