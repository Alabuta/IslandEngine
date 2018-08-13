/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: buffer objects initialization shader.
****
********************************************************************************************************************************/
#pragma include("Includes/ShaderVariables.glsl")
#pragma include("Includes/ShaderHelpers.glsl")


#pragma stage("vertex")

void main()
{
	gl_Position = TransformFromModelToClip(vec4(1.0));
}

#pragma stage("fragment")

void main()
{ }
