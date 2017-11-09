/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: http://www.dhpoware.com/demos/gl3FullScreenQuad.html
****
********************************************************************************************************************************/

#pragma stage("vertex")

layout(location = nVERTEX) in vec2 inVertex;

out vec2 texCoord;

void main()
{
    // inVertex is in normalized device coordinates (NDC).
    // The (x, y) coordinates are in the range [-1, 1].
    gl_Position = vec4(inVertex, 0.0, 1.0);

    // To generate the texture coordinates for the resulting full screen
    // quad we need to transform the vertex position's coordinates from the
    // [-1, 1] range to the [0, 1] range. This is achieved with a scale and
    // a bias.
    texCoord = inVertex * 0.5 + 0.5;
}

#pragma stage("fragment")

layout(early_fragment_tests) in;

layout(location = nFRAG_COLOR/*, index = 0*/) out vec4 FragColor;

layout(binding = 0) uniform sampler2D mainTexture;
layout(binding = 1) uniform sampler2D posTexture;
layout(binding = 2) uniform sampler2D normTexture;

in vec2 texCoord;

void main()
{
    FragColor.rgb = texture(normTexture, texCoord).rgb;
    FragColor.a = 1;
}
