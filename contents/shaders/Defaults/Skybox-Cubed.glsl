/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: default cube map skybox shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")

#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;

out vec3 texCoord;

void main()
{
    gl_Position = mProjView * vec4(inVertex, 0.0);

    // You can just assign w component to z,
    // but then you have to change depth comprasion function to GL_LEQUAL.
    gl_Position.z = gl_Position.w * 0.999999;

    texCoord = -inVertex;
}

#elif CRUS_FRAGMENT_SHADER

layout(location = nFRAG_COLOR, index = 0) out vec4 FragColor;
layout(location = nMAIN_COLOR) uniform vec4 mainColor = vec4(1.0);

uniform samplerCube skybox;

in vec3 texCoord;

void main()
{
    FragColor = texture(skybox, texCoord) * mainColor;
}

#endif