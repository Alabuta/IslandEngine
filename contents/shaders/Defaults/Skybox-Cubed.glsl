/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: default cube map skybox shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")

#pragma stage("vertex")

layout(location = nPOSITION) in vec3 inVertex;

out vec3 texCoord;

void main()
{
    gl_Position = TransformFromWorldToClip(vec4(inVertex, 0));

    // You can just assign w component to z,
    // but then you have to change depth comprasion function to GL_LEQUAL(GL_GEQUAL).
    // Instead, consider code example below.
#if CRUS_REVERSED_DEPTH
    gl_Position.z = gl_Position.w * 0.000001;
#else
    gl_Position.z = gl_Position.w * 0.999999;
#endif

    texCoord = -inVertex;
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

layout(location = nBASE_COLOR/*, index = 0*/) out vec4 fragColor;
layout(location = nMAIN_COLOR) uniform vec4 mainColor = vec4(1.0);

layout(binding = 0) uniform samplerCube skybox;

in vec3 texCoord;

void main()
{
    fragColor = texture(skybox, texCoord) * mainColor;
}
