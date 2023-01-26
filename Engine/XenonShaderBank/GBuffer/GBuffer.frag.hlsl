// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Common.hlsli"

#include "../Core/Texture.hlsli"

XENON_SETUP_TEXTURE(Texture2D, baseColor, 0)

struct FSOutput
{
	[[vk::location(0)]] float4 color : SV_TARGET0;
	[[vk::location(1)]] float4 normal : SV_TARGET1;
};

FSOutput main(VSOutput input)
{
	FSOutput output;
	output.color = baseColorTexture.Sample(baseColorSampler, input.textureCoordinate);
	output.normal = float4(input.normal, 1.0f);
	
	return output;
}