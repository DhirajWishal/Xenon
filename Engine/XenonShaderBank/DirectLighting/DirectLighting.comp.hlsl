// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "../Core/Common.hlsli"

#define LIGHT_LUT_HIDE_OUTPUT
#include "../LightLUT/Common.hlsli"

RWTexture2D<float4> resultImage : register(u0);

struct ControlStructure 
{
	uint m_LightCount;
};

cbuffer controlStructure : register(b1) { ControlStructure controlStructure; };
cbuffer lights : register(b2) { LightSource lights[XENON_MAX_LIGHT_SOURCE_COUNT]; };

/**
 * Setup an input image and it's sampler.
 *
 * @param name The image and sampler name. The iamge name will be <name>Image, and sampler will be <name>Sampler.
 * @param binding The image and sampler binding.
 */
#define SETUP_INPUT_IMAGE(name, binding)													\
	[[vk::combinedImageSampler]]															\
	Texture2D XENON_NAME_CONCAT(name, Image) : register(XENON_NAME_CONCAT(t, binding));		\
	[[vk::combinedImageSampler]]															\
	SamplerState XENON_NAME_CONCAT(name, Sampler) : register(XENON_NAME_CONCAT(s, binding))

SETUP_INPUT_IMAGE(positiveXColor, 3);
SETUP_INPUT_IMAGE(negativeXColor, 4);
SETUP_INPUT_IMAGE(positiveYColor, 5);
SETUP_INPUT_IMAGE(negativeYColor, 6);
SETUP_INPUT_IMAGE(positiveZColor, 7);
SETUP_INPUT_IMAGE(negativeZColor, 8);

SETUP_INPUT_IMAGE(positiveXNormal, 9);
SETUP_INPUT_IMAGE(negativeXNormal, 10);
SETUP_INPUT_IMAGE(positiveYNormal, 11);
SETUP_INPUT_IMAGE(negativeYNormal, 12);
SETUP_INPUT_IMAGE(positiveZNormal, 13);
SETUP_INPUT_IMAGE(negativeZNormal, 14);

SETUP_INPUT_IMAGE(positiveXPosition, 15);
SETUP_INPUT_IMAGE(negativeXPosition, 16);
SETUP_INPUT_IMAGE(positiveYPosition, 17);
SETUP_INPUT_IMAGE(negativeYPosition, 18);
SETUP_INPUT_IMAGE(positiveZPosition, 19);
SETUP_INPUT_IMAGE(negativeZPosition, 20);

cbuffer controlBlock : register(b21) { LightLUTControlBlock controlBlock; };
StructuredBuffer<float2> lookUpTable : register(t22);

bool isOccluded(float3 position, float3 lightPosition, uint index)
{
	const float3 delta = position - lightPosition;
	const float pitch = tan(delta.z / delta.x);
	const float yaw = tan(delta.y / delta.x);

	const float uniqueID = (pitch * 1000) + yaw;
	const float2 foundValue = lookUpTable[FindLookUpTableIndex(uniqueID, index * controlBlock.m_Stride, controlBlock.m_Stride)];

	const float lightDist = distance(lightPosition, position);
	return foundValue > lightDist;
}

[numthreads(8, 8, 1)]
void main(uint2 ThreadID : SV_DispatchThreadID)
{
	int2 coordinate = int2(ThreadID.xy);
	float3 normal = normalize(negativeZNormalImage[coordinate].xyz);
	float3 position = negativeZPositionImage[coordinate].xyz;

	// Iterate over the light sources and check if we're occluded by something.
	float4 litValue = float4(0.0f, 0.0f, 0.0f, 1.0f);
	const float4 colorValue = negativeZColorImage[coordinate];
	for(uint i = 0; i < controlStructure.m_LightCount; i++)
	{
		LightSource source = lights[i];

		float3 lightDir = normalize(source.m_Position - position);
		float diff = max(dot(normal, lightDir), 0.0f);
		float3 diffuse = diff * source.m_Color;

		if(!isOccluded(position, source.m_Position, i))
		{
			// litValue = float4(diffuse, 1.0f) * colorValue;
			litValue = colorValue;
		}
	}

	resultImage[coordinate] = litValue;
}