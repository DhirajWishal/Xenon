// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#ifndef SCENE_COMMON_HLSLI
#define SCENE_COMMON_HLSLI

struct VSOutput 
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float2 textureCoordinates : TEXCOORD0;
	[[vk::location(1)]] float3 normal : NORMAL0;
	[[vk::location(2)]] float3 viewVector : TEXCOORD1;
	[[vk::location(3)]] float3 lightVector : TEXCOORD2;
	[[vk::location(4)]] float4 shadowCoordinate : TEXCOORD3;
	[[vk::location(5)]] float4 lightColor : COLOR0;
};

#endif // SCENE_COMMON_HLSLI