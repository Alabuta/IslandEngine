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

#pragma stage("vertex")

layout(location = nVERTEX) in vec3 inVertex;
//layout(location = nNORMAL) in vec3 inNormal;
layout(location = nTEXCRD) in vec2 inTexCoord;

out from_vs_data {
    vec2 position;
    vec2 texCoord;
} vs_data;

void main()
{
    gl_Position = TransformFromModelToClip(vec4(inVertex, 1));

    vs_data.texCoord = inTexCoord;

    // Transform each vertex from clip space into viewport space.
    vs_data.position = normalizedToViewport(gl_Position.xy / gl_Position.w);
}

#pragma stage("geometry")

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in from_vs_data {
    vec2 position;
    vec2 texCoord;
} vs_data[];

out from_gs_data {
    noperspective vec2 position;
    vec2 texCoord;

    noperspective vec4 edgeA;
    noperspective vec4 edgeB;

    flat uint mask;
} gs_data;

const uint infoA[] = {0, 0, 0, 0, 1, 1, 2};
const uint infoB[] = {1, 1, 2, 0, 2, 1, 2};

const uint infoAd[] = {2, 2, 1, 1, 0, 0, 0};
const uint infoBd[] = {2, 2, 1, 2, 0, 2, 1};

void simpleCase()
{
    // Edge vectors of the transformed triangle.
    const vec2 ab = vs_data[1].position - vs_data[0].position;
    const vec2 bc = vs_data[2].position - vs_data[1].position;
    const vec2 ca = vs_data[0].position - vs_data[2].position;

    const float area = abs(fma(ab.x, ca.y, -ab.y * ca.x));

    // Vertices' heights.
    const float ha = area / length(bc);
    const float hb = area / length(ca);
    const float hc = area / length(ab);

    gs_data.edgeA = vec4(ha, 0.0, 0.0, 0.0);
    gs_data.texCoord = vs_data[0].texCoord;

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();


    gs_data.edgeA = vec4(0.0, hb, 0.0, 0.0);
    gs_data.texCoord = vs_data[1].texCoord;

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();


    gs_data.edgeA = vec4(0.0, 0.0, hc, 0.0);
    gs_data.texCoord = vs_data[2].texCoord;

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
}

void main()
{
    gs_data.mask = (gl_in[0].gl_Position.z < 0.0 ? 4 : 0) + (gl_in[1].gl_Position.z < 0.0 ? 2 : 0) + (gl_in[2].gl_Position.z < 0.0 ? 1 : 0);

    // All vertices behind the viewport.
    /*if (gs_data.mask == 7)
        return;*/

    if (gs_data.mask == 0) {
        simpleCase();
    }

    else {
        gs_data.edgeA.xy = vs_data[infoA[gs_data.mask]].position;
        gs_data.edgeB.xy = vs_data[infoB[gs_data.mask]].position;

        gs_data.edgeA.zw = normalize(gs_data.edgeA.xy - vs_data[infoAd[gs_data.mask]].position);
        gs_data.edgeB.zw = normalize(gs_data.edgeB.xy - vs_data[infoBd[gs_data.mask]].position);

        for (int i = 0; i < 3; ++i) {
            gs_data.position = vs_data[i].position;
            gs_data.texCoord = vs_data[i].texCoord;
            gl_Position = gl_in[i].gl_Position;

            EmitVertex();
        }
    }

    EndPrimitive();
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

layout(location = nFRAG_COLOR/*, index = 0*/) out vec4 FragColor;
layout(location = nMAIN_COLOR) uniform vec4 mainColor = vec4(0.8, 0.8, 0.8, 1.0);

layout(binding = 0) uniform sampler2D mainTexture;

uniform vec4 wireColor = vec4(0, 0.64, 0, 1.0);
uniform vec2 wireWidthAndFadeDistance = vec2(0.8, 0.4);

in from_gs_data {
    noperspective vec2 position;
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

    const float fading = clamp(wireWidthAndFadeDistance.y / gl_FragCoord.w, 0, 1);

    const float width = wireWidthAndFadeDistance.x;

    const float mix_val = clamp(smoothstep(width - 1, width + 1, dist) + fading, 0, 1);

    FragColor = mix(wireColor, mainColor, mix_val);
}
