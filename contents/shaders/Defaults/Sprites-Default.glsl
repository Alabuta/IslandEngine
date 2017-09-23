/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: default sprites shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;
layout(location = nNORMAL) in vec3 inNormal;
layout(location = nTEXCRD) in vec2 inTexCoord;

out vec3 position;
out vec2 vs_texCoord;

void main()
{
    position = vec3(mView * mModel * vec4(inVertex, 1.0));
    gl_Position = TransformFromModelToClip(inVertex);

    vs_texCoord = inTexCoord;
}

#elif CRUS_GEOMETRY_SHADER

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 positions[];
in vec2 vs_texCoord[];

out vec3 position;
out vec2 texCoord;

noperspective out vec3 edge_distance;

void main()
{
    // Transform each vertex into viewport space
    vec2 p0 = vec2(mViewport * (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec2 p1 = vec2(mViewport * (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec2 p2 = vec2(mViewport * (gl_in[2].gl_Position / gl_in[2].gl_Position.w));

    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);

    float alpha = acos((b * b + c * c - a * a) / (2.0 * b * c));
    float beta = acos((a * a + c * c - b * b) / (2.0 * a * c));

    float ha = abs(c * sin(beta));
    float hb = abs(c * sin(alpha));
    float hc = abs(b * sin(alpha));

    edge_distance = vec3(ha, 0, 0);
    position = positions[0];
    texCoord = vs_texCoord[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    edge_distance = vec3(0, hb, 0);
    position = positions[1];
    texCoord = vs_texCoord[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    edge_distance = vec3(0, 0, hc);
    position = positions[2];
    texCoord = vs_texCoord[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}

#elif CRUS_FRAGMENT_SHADER

layout(early_fragment_tests) in;

layout(location = nFRAG_COLOR/*, index = 0*/) out vec4 FragColor;
layout(location = nMAIN_COLOR) uniform vec4 mainColor = vec4(1.0);

layout(binding = 0) uniform sampler2D mainTexture;

in vec3 position;
in vec2 texCoord;

noperspective in vec3 edge_distance;

void main()
{
    vec4 color = texture2D(mainTexture, texCoord) * mainColor;

    // Find the smallest distance
    float d = min(edge_distance.x, edge_distance.y);
    d = min(d, edge_distance.z);

    float width = 1;

    /*float mix_val;

    if (d < width - 1) {
        mix_val = 1.0;
    }

    else if (d > width + 1) {
        mix_val = 0.0;
    }

    else {
        float x = d - (width - 1);
        mix_val = exp2(-2.0 * (x * x));
    }*/

    float mix_val = smoothstep(width - 1, width + 1, d);

    FragColor = mix(vec4(0, 1, 0, 1), color, mix_val);

    /*if (FragColor.a == 0.0)
        discard;*/
}

#endif