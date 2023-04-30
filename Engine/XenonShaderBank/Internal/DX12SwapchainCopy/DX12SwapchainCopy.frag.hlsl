// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

Texture2D baseColorTexture : register(t0);
SamplerState baseColorSampler : register(s0);		

struct FSInput 
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float2 textureCoordinate : TEXCOORD0;
};

float4 main(FSInput input) : SV_TARGET
{
	return baseColorTexture.Sample(baseColorSampler, input.textureCoordinate);
}