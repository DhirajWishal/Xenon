// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "../Core/VertexInputDefines.hlsli"
#include "../Core/Camera.hlsli"

struct VSInput 
{
	XENON_VERTEX_INPUT_VERTEX_POSITION float3 position : POSITION0;
	XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_0 float2 textureCoordinates : TEXCOORD0;
};

XENON_SETUP_CAMERA(MonoCamera, camera)

XENON_SETUP_TRANSFORM(transform);

struct VSOutput 
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float2 textureCoordinates : TEXCOORD0;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = mul(camera.projection, mul(camera.view, mul(transform.m_Matrix, float4(input.position, 1.0f))));
	output.textureCoordinates = input.textureCoordinates;

	return output;
}