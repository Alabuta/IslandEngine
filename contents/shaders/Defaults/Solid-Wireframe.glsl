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
    vs_data.position = mView * mModel * vec4(inVertex, 1.0);
    vs_data.texCoord = inTexCoord;

    gl_Position = mProj * vs_data.position;
}

#elif CRUS_GEOMETRY_SHADER

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in from_vs_data {
    vec4 position;
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

void simpleCase(in vec2 points[3])
{
    // Edge vectors of the transformed triangle.
    const vec2 a = points[1] - points[2];
    const vec2 b = points[2] - points[0];
    const vec2 c = points[1] - points[0];

    const float area = abs(b.x * c.y - b.y * c.x);

    // Vertices' heights.
    const float ha = area / length(a);
    const float hb = area / length(b);
    const float hc = area / length(c);

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

vec2 projToWindow(in vec4 position)
{
    const vec4 viewport = vec4(0, 0, 1920, 1080);

    return vec2(viewport.z * 0.5 * (position.x / position.w + 1.0) + viewport.x,
                viewport.w * 0.5 * (position.y / position.w + 1.0) + viewport.y);
}

void main()
{
    gs_data.mask = (gl_in[0].gl_Position.z < 0.0 ? 4 : 0) + (gl_in[1].gl_Position.z < 0.0 ? 2 : 0) + (gl_in[2].gl_Position.z < 0.0 ? 1 : 0);

    // All vertices behind the viewport.
    if (gs_data.mask == 7)
        return;

    vec2 points[3];

    // Transform each vertex into viewport space
    /*points[0] = (mViewport * (gl_in[0].gl_Position / gl_in[0].gl_Position.w)).xy;
    points[1] = (mViewport * (gl_in[1].gl_Position / gl_in[1].gl_Position.w)).xy;
    points[2] = (mViewport * (gl_in[2].gl_Position / gl_in[2].gl_Position.w)).xy;*/

    points[0] = projToWindow(gl_in[0].gl_Position / gl_in[0].gl_Position.w);
    points[1] = projToWindow(gl_in[1].gl_Position / gl_in[1].gl_Position.w);
    points[2] = projToWindow(gl_in[2].gl_Position / gl_in[2].gl_Position.w);

    if (gs_data.mask == 0) {
        simpleCase(points);
    }

    else {
        gs_data.edgeA.xy = points[infoA[gs_data.mask]];
        gs_data.edgeB.xy = points[infoB[gs_data.mask]];

        gs_data.edgeA.zw = normalize(gs_data.edgeA.xy - points[infoAd[gs_data.mask]]);
        gs_data.edgeB.zw = normalize(gs_data.edgeB.xy - points[infoBd[gs_data.mask]]);

        gs_data.position = points[0];
        gs_data.texCoord = vs_data[0].texCoord;
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();

        gs_data.position = points[1];
        gs_data.texCoord = vs_data[1].texCoord;
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();

        gs_data.position = points[2];
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

    const float width = 1.0;

    const float mix_val = smoothstep(width - 1, width + 1, dist);

    FragColor = mix(vec4(0, 0.64, 0, 1.0), mainColor, mix_val);

    //FragColor = vec4(0, 0.64, 0, exp2(-2.0 * dist * dist));

    /*if (FragColor.a == 0.0)
        discard;*/
}

#endif