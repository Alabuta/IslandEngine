/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: solid wireframe shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;
//layout(location = nNORMAL) in vec3 inNormal;
layout(location = nTEXCRD) in vec2 inTexCoord;

out from_vs_data {
    vec4 position;
    vec2 texCoord;
} vs_data;

void main()
{
    gl_Position = mView * mModel * vec4(inVertex, 1.0);

    vs_data.position = gl_Position;
    vs_data.texCoord = inTexCoord;

    gl_Position = mProj * gl_Position;
}

#elif CRUS_GEOMETRY_SHADER

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in from_vs_data {
    vec4 position;
    vec2 texCoord;
} vs_data[];

out from_gs_data {
    vec4 position;
    vec2 texCoord;

    noperspective vec4 edgeA;
    noperspective vec4 edgeB;

    flat uint mask;
} gs_data;

const uint infoA[] = {0, 0, 0, 0, 1, 1, 2};
const uint infoB[] = {1, 1, 2, 0, 2, 1, 2};

const uint infoAd[] = {2, 2, 1, 1, 0, 0, 0};
const uint infoBd[] = {2, 2, 1, 2, 0, 2, 1};

void simpleCase(in vec2 points[3])
{
    // Edge vectors of the transformed triangle.
    const float a = length(points[1] - points[2]);
    const float b = length(points[2] - points[0]);
    const float c = length(points[1] - points[0]);

#if 1
    const float p = (a + b + c) * 0.5;
    const float root = 2 * sqrt(p * (p - a) * (p - b) * (p - c));

    // Vertices' heights.
    const float ha = root / a;
    const float hb = root / b;
    const float hc = root / c;

#else
    const float inv2c = 0.5 / c;

    const float aa_minus_bb = a * a - b * b;
    const float cc = c * c;

    const float alpha = acos(inv2c * (cc - aa_minus_bb) / b);
    const float beta = acos(inv2c * (aa_minus_bb + cc) / a);

    const float ha = abs(c * sin(beta));

    const float sin_alpha = sin(alpha);
    const float hb = abs(c * sin_alpha);
    const float hc = abs(b * sin_alpha);
#endif


    gs_data.edgeA = vec4(ha, 0.0, 0.0, 0.0);
    gs_data.position = vs_data[0].position;
    gs_data.texCoord = vs_data[0].texCoord;

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();


    gs_data.edgeA = vec4(0.0, hb, 0.0, 0.0);
    gs_data.position = vs_data[1].position;
    gs_data.texCoord = vs_data[1].texCoord;

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();


    gs_data.edgeA = vec4(0.0, 0.0, hc, 0.0);
    gs_data.position = vs_data[2].position;
    gs_data.texCoord = vs_data[2].texCoord;

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
}

void main()
{
    gs_data.mask = (gl_in[0].gl_Position.z < 0.0 ? 4 : 0) + (gl_in[1].gl_Position.z < 0.0 ? 2 : 0) + (gl_in[2].gl_Position.z < 0.0 ? 1 : 0);

    // All vertices behind the viewport.
    if (gs_data.mask == 7)
        return;

    vec2 points[3];

    // Transform each vertex into viewport space
    points[0] = (mViewport * (gl_in[0].gl_Position / gl_in[0].gl_Position.w)).xy;
    points[1] = (mViewport * (gl_in[1].gl_Position / gl_in[1].gl_Position.w)).xy;
    points[2] = (mViewport * (gl_in[2].gl_Position / gl_in[2].gl_Position.w)).xy;

    if (gs_data.mask == 0) {
        simpleCase(points);
    }

    else {
        gs_data.edgeA.xy = points[infoA[gs_data.mask]];
        gs_data.edgeB.xy = points[infoB[gs_data.mask]];

        gs_data.edgeA.zw = normalize(gs_data.edgeA.xy - points[infoAd[gs_data.mask]]);
        gs_data.edgeB.zw = normalize(gs_data.edgeB.xy - points[infoBd[gs_data.mask]]);

        gs_data.position = vs_data[0].position;
        gs_data.texCoord = vs_data[0].texCoord;
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();

        gs_data.position = vs_data[1].position;
        gs_data.texCoord = vs_data[1].texCoord;
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();

        gs_data.position = vs_data[2].position;
        gs_data.texCoord = vs_data[2].texCoord;
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}

#elif CRUS_FRAGMENT_SHADER

layout(early_fragment_tests) in;

layout(location = nFRAG_COLOR/*, index = 0*/) out vec4 FragColor;
layout(location = nMAIN_COLOR) uniform vec4 mainColor = vec4(0.8, 0.8, 0.8, 1.0);

layout(binding = 0) uniform sampler2D mainTexture;

in from_gs_data {
    vec4 position;
    vec2 texCoord;

    noperspective vec4 edgeA;
    noperspective vec4 edgeB;

    flat uint mask;
} gs_data;

float getDistanceToEdges()
{
    float dist = 0.0;

    if (gs_data.mask == 0) {
        dist = min(min(gs_data.edgeA.x, gs_data.edgeA.y), gs_data.edgeA.z);
    }

    else {
        const vec2 AF = gs_data.position.xy - gs_data.edgeA.xy;
        const float sqAF = dot(AF, AF);
        const float AFcosA = dot(AF, gs_data.edgeA.zw);

        dist = abs(sqAF - AFcosA * AFcosA);

        const vec2 BF = gs_data.position.xy - gs_data.edgeB.xy;
        const float sqBF = dot(BF, BF);
        const float BFcosB = dot(BF, gs_data.edgeB.zw);

        dist = min(dist, abs(sqBF - BFcosB * BFcosB));

        if (gs_data.mask == 1 || gs_data.mask == 2 || gs_data.mask == 4) {
            const float AFcosA0 = dot(AF, normalize(gs_data.edgeB.xy - gs_data.edgeA.xy));
            dist = min(dist, abs(sqAF - AFcosA0 * AFcosA0));
        }

        dist = sqrt(dist);
    }

    return dist;
}

void main()
{
    // Find the smallest distance
    const float dist = getDistanceToEdges();

    const float width = 1.5;

    const float mix_val = smoothstep(width - 1, width + 1, dist);

    FragColor = mix(vec4(0, 0.64, 0, 1.0), mainColor, mix_val);

    /*if (FragColor.a == 0.0)
        discard;*/
}

#endif