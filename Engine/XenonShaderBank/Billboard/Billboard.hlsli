// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#ifndef BILLBOARD_HLSLI
#define BILLBOARD_HLSLI

struct VSOutput 
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float2 textureCoordinates : TEXCOORD0;
};

#endif BILLBOARD_HLSLI