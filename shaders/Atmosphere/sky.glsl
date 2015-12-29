/****************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    9th September 2012 / 30th June 2010.
****    Description: sky render shader file.
****
****************************************************************************************/
#if CRUS_VERTEX_SHADER

layout(location = nVERTEX) in vec3 inVertex;

uniform mat4 projInverse;
uniform mat4 viewInverse;

layout(row_major) uniform TRSM {
    mat4 ProjMatrix;
    mat4 ViewMatrix;

    mat4 ViewProj;
    mat4 ModelView;
};

out vec3 ray;

void main()
{
    ray = -(viewInverse * vec4((projInverse * vec4(inVertex, 1.0)).xyz, 0.0)).xyz;
    //ray = (ViewProj * vec4(inVertex, 1.0)).xyz;

    gl_Position = vec4(inVertex.xy, 0.9999999, 1.0);
    //gl_Position = ViewProj * vec4(inVertex, 1.0);
}

#elif CRUS_FRAGMENT_SHADER

uniform vec3 worldCamera;
uniform vec3 worldSunDir;

in vec3 ray;

out vec4 FragColor;

void main()
{
    vec3 view = normalize(ray);
    vec3 sunColor = vec3(step(cos(3.1415926 / 180.0), dot(view, worldSunDir))) * SUN_INTENSITY;

    vec3 extinction;
    vec3 inscatter = skyRadiance(worldCamera + earthPos, view, worldSunDir, extinction);

    vec3 final = sunColor * extinction + inscatter;

    FragColor.rgb = hdr(final);
    FragColor.a = 1.0;
}

#endif