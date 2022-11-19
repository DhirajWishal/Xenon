#version 450

layout (location = 0) in vec2 inTexCoordinates;

layout (location = 0) out vec4 outFragColor;

layout (binding = 1) uniform sampler2D baseColorTexture;

void main()
{    
   outFragColor = texture(baseColorTexture, inTexCoordinates);
}