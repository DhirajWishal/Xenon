// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Billboard.hlsli"
#include "../Core/Texture.hlsli"

XENON_SETUP_TEXTURE(Texture2D, baseColor, 0)

float4 main(VSOutput input) : SV_TARGET
{
	return baseColorTexture.Sample(baseColorSampler, input.textureCoordinates);
}