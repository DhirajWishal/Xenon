// Copyright 2020 Google LLC

#include "VertexInputDefines.hlsli"

struct VSInput
{
	[[vk::location(XENON_VERTEX_INPUT_VERTEX_POSITION_LOCATION)]] float2 Pos : POSITION0;
	[[vk::location(XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_0_LOCATION)]] float2 UV : TEXCOORD0;
	[[vk::location(XENON_VERTEX_INPUT_VERTEX_COLOR_0_LOCATION)]] float4 Color : COLOR0;
};

struct PushConstants
{
	float2 scale;
	float2 translate;
};

[[vk::push_constant]]
PushConstants pushConstants;

struct VSOutput
{
	float4 Pos : SV_POSITION;
	[[vk::location(0)]] float2 UV : TEXCOORD0;
	[[vk::location(1)]] float4 Color : COLOR0;
};

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;
	output.UV = input.UV;
	output.Color = input.Color;
	output.Pos = float4(input.Pos * pushConstants.scale + pushConstants.translate, 0.0, 1.0);
	return output;
}