#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 11) in vec2 inTextureCoordinates;

layout(location = 0) out vec2 outTextureCoordinates;

void main()
{
	gl_Position = vec4(inPosition, 0.0f, 1.0f);
	outTextureCoordinates = inTextureCoordinates;
} 