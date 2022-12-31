// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "../Core/Texture.hlsli"

XENON_SETUP_TEXTURE(Texture2D, baseColor, 0)

struct FSInput 
{
	[[vk::location(0)]] float2 textureCoordinate : TEXCOORD0;
};

float4 main(FSInput input) : SV_TARGET
{
	return baseColorTexture.Sample(baseColorSampler, input.textureCoordinate);
}