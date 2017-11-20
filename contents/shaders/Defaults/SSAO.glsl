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

/*layout(location = 0)*/ uniform vec4 lightPosition = vec4(10, 10, 10, 0);

subroutine void RenderPassType();
layout(location = 0) subroutine uniform RenderPassType RenderPass;

out vec4 normal;
out vec4 position;
out float depth;
out vec3 ray;
out vec4 light;

out vec2 texCoord;

layout(index = 0) subroutine (RenderPassType)
void modelProccessing()
{
    normal = normalize(mNormal * vec4(inNormal, 0));
    // normal.xyz = normalize(transpose(inverse(mat3(mView * mModel))) * inNormal);

    position = TransformFromModelToView(vec4(inVertex, 1));

    if (lightPosition.w == 0)
        light = normalize(mView * lightPosition);

    else light = normalize(mView * lightPosition - position);

    gl_Position = TransformFromViewToClip(position);
    depth = gl_Position.z / gl_Position.w;
}

layout(index = 1) subroutine(RenderPassType)
void fullQuadRender()
{
    gl_Position = vec4(inVertex.xy, 1.0, 1.0);
    texCoord = inVertex.xy * 0.5 + 0.5;
    
    ray = gl_Position.xyz;
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
#define nFRAG_DEPTH 3

layout(location = nFRAG_COLOR) out vec4 FragColor;
layout(location = nFRAG_POSITION) out float FragPosition;
layout(location = nFRAG_NORMAL) out vec2 FragNormal;
//layout(location = nFRAG_DEPTH) out float FragDepth;

layout(binding = 0) uniform sampler2D colorSampler;
layout(binding = 1) uniform sampler2D positionTexture;
layout(binding = 2) uniform sampler2D normalSampler;
layout(binding = 3) uniform sampler2D depthSampler;
layout(binding = 4) uniform sampler2D noiseSampler;

/* layout(location = nMAIN_COLOR) uniform */vec4 mainColor = vec4(1.0); // vec4(0, 0.74609375, 1, 1);

layout(location = 4) uniform mat4 projection;
layout(location = 8) uniform vec3 samples[64];

const vec2 noiseScale = vec2(1920.0 / 4.0, 1080.0 / 4.0);
const float radius = 0.5;
const float bias = 0.025;
const float kernelSize = 32;
const float epsilon = 0.00001;

const float zNear = 0.01, zFar = 10.0;

in vec4 normal;
in vec4 position;
in float depth;
in vec3 ray;
in vec4 light;

in vec2 texCoord;

layout(index = 0) subroutine(RenderPassType)
void renderGBuffer()
{
    vec3 n = normalize(normal.xyz);
    vec3 p = position.xyz;

    //vec4 xyzw = vec4((gl_FragCoord.xy / vec2(1920, 1080)) * 2 - 1, (depth.x / depth.y) * 2 - 1, 1);
    //vec4 xyzw = vec4((gl_FragCoord.xy / vec2(1920, 1080)) * 2 - 1, gl_FragCoord.z * 2 - 1, 1);

    /*xyzw = inverse(projection) * xyzw;
    xyzw /= xyzw.w;*/
    //FragColor.rgb = xyzw.xyz;
    //FragColor.rgb = position.xyz;
    float diffuse = dot(n, normalize(light.xyz));
    FragColor.rgb = vec3(diffuse * 0.5 + 0.5);

    /*if (abs(position.x - gl_FragCoord.z * gl_FragCoord.w) < 0.91) // (gl_FragCoord.z / gl_FragCoord.w) = depth
        FragColor.rgb = vec3(0, 1, 0);

    else FragColor.rgb = vec3(1, 0, 0);*/

    FragColor.a = 1;

    //FragColor = mainColor;
    FragPosition = position.z;// vec4(position.xyz, 0);
    FragNormal = n.xy;// vec4(n.xy, 0, 0);
    //FragDepth = gl_FragCoord.z;// vec4((gl_FragCoord.z / gl_FragCoord.w) * 2 - 1, 0, 0, 0);
}

vec3 getPositionFromDepth(vec2 depthTexCoord)
{
    vec2 xy = depthTexCoord * 2 - 1;
    float z = texture(depthSampler, depthTexCoord).x;

    vec4 projectedPosition = vec4(xy, z, 1);

    vec4 vertexPosition = inverse(projection) * projectedPosition;

    return vertexPosition.xyz / vertexPosition.w;
}

layout(index = 1) subroutine(RenderPassType)
void ssao()
{
    //FragColor.rgb = getPositionFromDepth(texCoord);
    //FragColor.rgb = vec3(texture(colorSampler, texCoord).xyz);// *0.5 + 0.5;
    /*FragColor.rgb = vec3(texture(positionTexture, texCoord).z) * 0.5 + 0.5;
    FragColor.rgb = vec3(texture(normalSampler, texCoord).xyz);
    FragColor.z = sqrt(fma(-FragColor.y, FragColor.y, fma(FragColor.x, -FragColor.x, 1)));*/

#if 1
    float d = (2 * zNear * zFar / (zFar + zNear - (zFar - zNear) * texture(depthSampler, texCoord).x));
    //FragColor.rgb = -vec3(texture(positionTexture, texCoord).z) / (zFar - zNear);
    //FragColor.rgb = -vec3(texture(positionTexture, texCoord).z) / zFar;
    FragColor.rgb = vec3(d / zFar);
    //FragColor.b = 0;
    /*FragColor.rgb = texture(positionTexture, texCoord).xyz;
    FragColor.b *= -1;*/
    //FragColor.rgb = FragColor.rgb * 0.5 + 0.5;
#endif

#if 0
    vec3 normal = vec3(texelFetch(normalSampler, ivec2(gl_FragCoord.xy), 0).xy, 0);
    //vec3 normal = vec3(texture(normalSampler, texCoord).xy, 0);
    normal.z = sqrt(fma(-normal.y, normal.y, fma(normal.x, -normal.x, 1)));
    FragColor.rgb = normal;
#endif

    //FragColor.rgb = vec3(texture(colorSampler, texCoord).xyz);

    /*if (abs(FragColor.z - 4.0) < epsilon) // (gl_FragCoord.z / gl_FragCoord.w) = depth
        FragColor.rgb = vec3(0, 1, 0);

    else FragColor.rgb = vec3(1, 0, 0);*/

    //FragColor.rgb *= 0.1;

    //FragColor.rgb = vec3(texture(depthSampler, texCoord)); // gl_FragCoord.xy / vec2(1920, 1080)
#if 0
    // FragColor.rgb = samples[int(round(texCoord.x * 63))];
    vec3 nn = texture(normalSampler, texCoord).xyz;

    vec3 p = texture(positionTexture, texCoord).xyz;
    //p.z = nn.z;
    //p.xy = ;
    //p.x = getPositionFromDepth(texCoord).xy;

    vec3 n = vec3(nn.xy, 0);// texture(normalSampler, texCoord).rgb;
    n.z = sqrt(fma(-n.y, n.y, fma(n.x, -n.x, 1)));

    vec3 rvec = texture(noiseSampler, texCoord * noiseScale).rgb;

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
#endif
    FragColor.a = 1;
}

layout(index = 2) subroutine(RenderPassType)
void render()
{
    FragColor.rgb = texture(colorSampler, texCoord).xyz;
    /*vec2 texelSize = 1.0 / vec2(textureSize(colorSampler, 0));
    float result = 0;

    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(colorSampler, texCoord + offset).x;
        }
    }

    FragColor.rgb = vec3(result / (4.0 * 4.0));*/
    FragColor.a = 1;
}

void main()
{
    RenderPass();
}