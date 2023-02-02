// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#ifndef GBUFFER_COMMON_HLSLI
#define GBUFFER_COMMON_HLSLI

struct VSOutput 
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float2 textureCoordinate : TEXCOORD0;
	[[vk::location(1)]] float3 normal : NORMAL0;
};

#endif // GBUFFER_COMMON_HLSLI