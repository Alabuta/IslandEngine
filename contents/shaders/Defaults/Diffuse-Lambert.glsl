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

layout(location = nVERTEX) in vec3 inVertex;
layout(location = nNORMAL) in vec3 inNormal;

/*layout(location = 0)*/ uniform vec4 lightPosition = vec4(10, 10, 10, 0);

out vec4 light;
out vec4 normal;
out vec4 position;

void main()
{
    normal = normalize(mNormal * vec4(inNormal, 0));
    // normal.xyz = normalize(transpose(inverse(mat3(mView * mModel))) * inNormal);

    position = TransformFromModelToView(vec4(inVertex, 1));

    if (lightPosition.w == 0)
        light = normalize(mView * lightPosition);

    else light = normalize(mView * lightPosition - position);

    gl_Position = TransformFromViewToClip(position);
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

layout(location = nFRAG_COLOR/*, index = 0*/) out vec4 FragColor;
layout(location = nMAIN_COLOR) uniform vec4 mainColor = vec4(1.0); // vec4(0, 0.74609375, 1, 1);

in vec4 light;
in vec4 normal;
in vec4 position;

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

    FragColor = vec4(mainColor.xyz * diffuse, mainColor.a);
    // FragColor = vec4(n.xyz, 1);
}
