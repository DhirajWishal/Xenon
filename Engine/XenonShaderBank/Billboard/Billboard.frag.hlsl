// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "Billboard.hlsli"
#include "../Core/Texture.hlsli"

XENON_SETUP_TEXTURE(Texture2D, baseColor, 0)

float4 main(VSOutput input) : SV_TARGET
{
	float4 color = baseColorTexture.Sample(baseColorSampler, input.textureCoordinates);

	// For now discard the pixel if the color has some transparency.
	if(color.a < 1.0f)
		discard;

	return color;
}