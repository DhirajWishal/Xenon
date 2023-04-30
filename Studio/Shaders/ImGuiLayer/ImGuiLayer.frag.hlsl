// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "Core/Texture.hlsli"

XENON_SETUP_TEXTURE(Texture2D, baseColor, 0)

struct FSInput 
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float2 UV : TEXCOORD0;
	[[vk::location(1)]] float4 color : COLOR0;
};

float4 main(FSInput input) : SV_TARGET
{
	return input.color * baseColorTexture.Sample(baseColorSampler, input.UV);
}