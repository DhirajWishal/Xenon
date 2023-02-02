// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Common.hlsli"

#include "../Core/Texture.hlsli"

XENON_SETUP_TEXTURE(Texture2D, baseColor, 0)

struct FSOutput
{
	[[vk::location(0)]] float4 color : SV_TARGET0;
	[[vk::location(1)]] float4 normal : SV_TARGET1;
	[[vk::location(2)]] float4 position : SV_TARGET2;
};

float linearDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * 0.001f * 256.0f) / (256.0f + 0.001f - z * (256.0f - 0.001f));
}

FSOutput main(VSOutput input)
{
	FSOutput output;
	output.color = baseColorTexture.Sample(baseColorSampler, input.textureCoordinate);
	output.normal = float4(input.normal, 1.0f);
	output.position = float4(input.position.xyz, linearDepth(input.position.z));
	
	return output;
}