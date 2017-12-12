/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: simple lambert model shader.
****
********************************************************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#pragma stage("vertex")

layout(location = nPOSITION) in vec3 inVertex;
layout(location = nNORMAL) in vec3 inNormal;
layout(location = nTEX_COORD) in vec2 inTexCoord;

/*layout(location = 0)*/ uniform vec4 lightPosition = vec4(10, 10, 10, 0);

out vec4 light;
out vec4 normal;
out vec2 texCoord;

void main()
{
    texCoord = inTexCoord;

    normal = normalize(mNormal * vec4(inNormal, 0));

    vec4 position = TransformFromModelToView(vec4(inVertex, 1));

    if (lightPosition.w == 0)
        light.xyz = normalize(TransformFromWorldToView(lightPosition).xyz);

    else light.xyz = normalize(TransformFromWorldToView(lightPosition).xyz - position.xyz);

    gl_Position = TransformFromViewToClip(position);
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

layout(location = nBASE_COLOR) out vec4 fragColor;
layout(location = nNORMALS) out vec2 fragNormal;

/*layout(location = nMAIN_COLOR)*/ uniform vec4 mainColor = vec4(1.0); // vec4(0, 0.74609375, 1, 1);

layout(binding = 0) uniform sampler2D mainTexture;

in vec4 light;
in vec4 normal;
in vec2 texCoord;

#define HALF_LAMBERT 1
#define WRAPPED_AROUND_LAMBERT 0

void main()
{
    vec3 n = normalize(normal.xyz);
    vec3 l = normalize(light.xyz);

#if HALF_LAMBERT
    float diffuse = (dot(n, l) * 0.5 + 0.5);
#elif WRAPPED_AROUND_LAMBERT
    const float offset = 0.5;
    float diffuse = max(dot(n, l) + offset, 0) / (1 + offset);
#else
    float diffuse = dot(n, l);
#endif

    fragColor = vec4(mainColor.rgb * diffuse, mainColor.a);
    fragNormal = n.xy;
}
