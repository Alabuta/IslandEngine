/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: screen space ambient oclusion shader.
****
********************************************************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#pragma stage("vertex")

layout(location = nVERTEX) in vec3 inVertex;
layout(location = nNORMAL) in vec3 inNormal;

subroutine void RenderPassType();
layout(location = 0) subroutine uniform RenderPassType RenderPass;

out vec4 normal;
out vec4 position;

out vec2 texCoord;

layout(index = 0) subroutine (RenderPassType)
void modelProccessing()
{
    normal = normalize(mNormal * vec4(inNormal, 0));
    // normal.xyz = normalize(transpose(inverse(mat3(mView * mModel))) * inNormal);

    position = TransformFromModelToView(vec4(inVertex, 1));

    gl_Position = TransformFromViewToClip(position);
}

layout(index = 1) subroutine(RenderPassType)
void fullQuadRender()
{
    gl_Position = vec4(inVertex.xy, 0.0, 1.0);
    texCoord = inVertex.xy * 0.5 + 0.5;
}

void main()
{
    RenderPass();
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

subroutine void RenderPassType();
layout(location = 0) subroutine uniform RenderPassType RenderPass;

#define nFRAG_POSITION 1
#define nFRAG_NORMAL 2

layout(location = nFRAG_COLOR) out vec4 FragColor;
layout(location = nFRAG_POSITION) out vec4 FragPosition;
layout(location = nFRAG_NORMAL) out vec4 FragNormal;

layout(binding = 0) uniform sampler2D mainTexture;
layout(binding = 1) uniform sampler2D positionTexture;
layout(binding = 2) uniform sampler2D normalTexture;

/* layout(location = nMAIN_COLOR) uniform */vec4 mainColor = vec4(1.0); // vec4(0, 0.74609375, 1, 1);

in vec4 normal;
in vec4 position;

in vec2 texCoord;

layout(index = 0) subroutine(RenderPassType)
void renderGBuffer()
{
    vec3 n = normalize(normal.xyz);
    vec3 p = normalize(position.xyz);

    FragColor = mainColor;
    FragPosition = vec4(p, 1);
    FragNormal = vec4(n, 1);
}

layout(index = 1) subroutine(RenderPassType)
void render()
{
    FragColor = texture(normalTexture, texCoord);
    FragColor.a = 1;
}

void main()
{
    RenderPass();
}