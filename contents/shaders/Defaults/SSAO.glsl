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
layout(binding = 3) uniform sampler2D noiseTexture;

/* layout(location = nMAIN_COLOR) uniform */vec4 mainColor = vec4(1.0); // vec4(0, 0.74609375, 1, 1);

layout(location = 4) uniform mat4 projection;
layout(location = 8) uniform vec3 samples[64];

const vec2 noiseScale = vec2(1920.0 / 4.0, 1080.0 / 4.0);
const float radius = 0.5;
const float bias = 0.025;
const float kernelSize = 32;

in vec4 normal;
in vec4 position;

in vec2 texCoord;

layout(index = 0) subroutine(RenderPassType)
void renderGBuffer()
{
    vec3 n = normalize(normal.xyz);
    vec3 p = position.xyz;

    FragColor = mainColor;
    FragPosition = vec4(p, 1);
    FragNormal = vec4(n.xy, 0, 0);
}

layout(index = 1) subroutine(RenderPassType)
void ssao()
{
    FragColor = texture(mainTexture, texCoord);
    FragColor.a = 1;
    // FragColor.rgb = samples[int(round(texCoord.x * 63))];

    vec4 nn = texture(normalTexture, texCoord);

    vec3 p = texture(positionTexture, texCoord).xyz;
    //p.z = nn.z;

    vec3 n = vec3(nn.xy, 0);// texture(normalTexture, texCoord).rgb;
    n.z = sqrt(fma(-n.y, n.y, fma(n.x, -n.x, 1)));

    vec3 rvec = texture(noiseTexture, texCoord * noiseScale).rgb;

    vec3 t = normalize(rvec - n * dot(rvec, n));
    vec3 b = cross(n, t);
    mat3 TBN = mat3(t, b, n);

    float occlusion = 0;

    for (int i = 0; i < kernelSize; ++i) {
        vec3 s = TBN * samples[i];
        s = p + s * radius;

        vec4 o = projection * vec4(s, 1);
        o.xyz /= o.w;
        o.xyz = fma(o.xyz, vec3(0.5), vec3(0.5));

        float d = texture(positionTexture, o.xy).z;

        float rangeCheck = smoothstep(0, 1, radius / abs(p.z - d));

        occlusion += d < s.z + bias ? 0 : rangeCheck;
    }

    occlusion = 1 - occlusion / kernelSize;
    FragColor.rgb *= occlusion;
}

layout(index = 2) subroutine(RenderPassType)
void render()
{
    vec2 texelSize = 1.0 / vec2(textureSize(mainTexture, 0));
    float result = 0;

    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(mainTexture, texCoord + offset).x;
        }
    }

    FragColor.rgb = vec3(result / (4.0 * 4.0));
    FragColor.a = 1;
}

void main()
{
    RenderPass();
}