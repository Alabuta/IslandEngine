/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 28th October 2010.
****    Description: test shader file.
****
********************************************************************************************************************************/
//#extension GL_ARB_conservative_depth : enable

//const float Fcoef = 2.0 / log2(4194304.0 + 1.0);

#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;
layout(location = nTEXCRD) in vec2 inTexCoord;
uniform vec2 inAnimTexCoord;
//in vec2 inTexCoord;
out vec2 texCoord;

layout(binding = nTRANSFORM, row_major) uniform TRSM {
    mat4 mModel;
    mat4 mNormalMatrix; // Inverted and transposed model (world) matrix.

    mat4 mViewProj;

    mat4 mView;
    mat4 mProj;
};

out float logz;

void main()
{
    gl_Position = mViewProj * mModel * vec4(inVertex, 1.0);

    //logz = 1.0 + gl_Position.z;
    //gl_Position.z = log2(max(1e-6, logz)) * Fcoef - 1.0;
    //gl_Position.z = 2.0*log2(gl_Position.w/0.1)/log2(4194304.0/0.1) - 1;

    texCoord = inTexCoord + inAnimTexCoord;
}

#elif CRUS_FRAGMENT_SHADER

layout(binding = nMATERIAL, shared) uniform CMTS {
    vec4 Clr0;
    vec4 Clr1;
    vec4 Clr2;
    vec4 Clr3;
};

layout(location = nFRAG_COLOR, index = 0) out vec4 FragColor;
layout(depth_less) out float gl_FragDepth;

in float logz;

uniform sampler2D albedoTexture;
in vec2 texCoord;

void main()
{
    //FragColor = vec4(pow(Clr0.rgb, vec3(1.0 / 2.2)), Clr0.a);
    //FragColor = vec4(sqrt(Clr0.rgb), Clr0.a);
    //FragColor = Clr0;
    FragColor = texture2D(albedoTexture, texCoord);

    //gl_FragDepth = log2(logz) * 0.5 * Fcoef;
}

#endif