/****************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: default cube map skybox shader file.
****
****************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")

#pragma stage("vertex")

layout(location = nPOSITION) in vec3 inVertex;

const int kSIDES = 6;



layout(location = 8) uniform mat4 projectionMatrix = mat4(1.);
layout(location = 9) uniform mat4 viewMatrix = mat4(1.);

//layout(binding = 8, std430) readonly buffer VIEW_MATRICES
//{
//    mat4 viewMatrices[kSIDES];
//};

out vec3 texCoord;

subroutine void RenderPassType();
layout(location = 0) subroutine uniform RenderPassType RenderPass;


layout(index = 0) subroutine(RenderPassType)
void equirectangularMapRender()
{
    gl_Position = camera.projectionView * object.world * vec4(inVertex, 0.);

    // You can just assign w component to z,
    // but then you have to change depth comprasion function to GL_LEQUAL(GL_GEQUAL).
    // Instead, consider code example below.
#if CRUS_REVERSED_DEPTH
    gl_Position.z = gl_Position.w * .000001;
#else
    gl_Position.z = gl_Position.w * .999999;
#endif

    texCoord = inVertex;
}

layout(index = 1) subroutine(RenderPassType)
void cubeMapRender()
{
    gl_Position = projectionMatrix * viewMatrix * vec4(inVertex, 0);

    // You can just assign w component to z,
    // but then you have to change depth comprasion function to GL_LEQUAL(GL_GEQUAL).
    // Instead, consider code example below.
#if CRUS_REVERSED_DEPTH
    gl_Position.z = gl_Position.w * .000001;
#else
    gl_Position.z = gl_Position.w * .999999;
#endif

    texCoord = inVertex;
}



layout(index = 2) subroutine(RenderPassType)
void fullQuadRender()
{
    gl_Position = vec4(inVertex.xy, 1., 1.);
    texCoord.xy = inVertex.xy * .5 + .5;
}

void main()
{
    RenderPass();
}

#pragma stage("fragment")

subroutine void RenderPassType();
layout(location = 0) subroutine uniform RenderPassType RenderPass;

layout(early_fragment_tests) in;

layout(location = nBASE_COLOR) out vec4 fragColor;

layout(binding = nALBEDO) uniform sampler2D equirectangularMap;
layout(binding = nNORMAL_MAP) uniform samplerCube cubeMap;
//layout(bindless_sampler, location = nNORMAL_MAP) uniform samplerCube cubeMap;
layout(binding = 3) uniform sampler2D map;


in vec3 texCoord;


const vec2 invAtan = vec2(.1591, .3181);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));

    return uv * invAtan + .5;
}

layout(index = 0) subroutine(RenderPassType)
void equirectangularMapRender()
{
    vec2 uv = SampleSphericalMap(normalize(texCoord));
    vec3 envColor = texture(equirectangularMap, uv).rgb;

    fragColor = vec4(envColor, 1.);
}

layout(index = 1) subroutine(RenderPassType)
void cubeMapRender()
{
    vec3 envColor = texture(cubeMap, normalize(texCoord)).rgb;

    envColor = envColor / (envColor + vec3(1.));
    envColor = pow(envColor, vec3(1. / 2.2));

    fragColor = vec4(envColor, 1.);
}

layout(index = 2) subroutine(RenderPassType)
void quadRender()
{
    vec3 envColor = texture(map, texCoord.xy).rgb;

    /*envColor = envColor / (envColor + vec3(1.));
    envColor = pow(envColor, vec3(1.0 / 2.2));*/

    fragColor = vec4(envColor, 1.);
}


const float PI = 3.14159265359;
const float sampleDelta = PI / 100.;

layout(index = 3) subroutine(RenderPassType)
void convolution()
{
    vec3 normal = normalize(texCoord);
    vec3 irradiance = vec3(0.);

    vec3 up = vec3(0., 1., 0.);
    vec3 right = cross(up, normal);
    up = cross(normal, right);

    int samplesNumber = 0;

    float cosPhi = 0., sinPhi = 0.;
    float cosTheta = 0., sinTheta = 0.;

    for (float phi = 0.; phi < 2. * PI; phi += sampleDelta) {
        cosPhi = cos(phi);
        sinPhi = sin(phi);

        for (float theta = 0.; theta < .5 * PI; theta += sampleDelta) {
            cosTheta = cos(theta);
            sinTheta = sin(theta);

            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);

            // tangent space to world
            vec3 sampleVector = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += texture(cubeMap, sampleVector).rgb * cosTheta * sinTheta;

            ++samplesNumber;
        }
    }

    irradiance = PI * irradiance / float(samplesNumber);

    fragColor = vec4(irradiance, 1.);
}

void main()
{
    RenderPass();
}
