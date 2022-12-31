// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/VertexInputDefines.hlsli"
#include "Core/Camera.hlsli"

struct VSInput 
{
	XENON_VERTEX_INPUT_VERTEX_POSITION float2 position : POSITION;
	XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_0 float2 UV : TEXCOORD0;
	XENON_VERTEX_INPUT_VERTEX_COLOR_0 float4 color : COLOR0;
};

struct UserData 
{
	float2 scale;
	float2 translate;
};

cbuffer userData : register(b0) { UserData userData; }

struct VSOutput
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float2 UV : TEXCOORD0;
	[[vk::location(1)]] float4 color : COLOR0;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.UV = input.UV;
	output.color = input.color;
	output.position = float4(input.position * userData.scale + userData.translate, 0.0f, 1.0f);

	return output;
}