// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "Common.hlsli"

#include "../Core/VertexInputDefines.hlsli"
#include "../Core/Camera.hlsli"

struct VSInput 
{
	XENON_VERTEX_INPUT_VERTEX_POSITION float3 position : POSITION0;
	XENON_VERTEX_INPUT_VERTEX_NORMAL float3 normal : NORMAL0;
	XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_0 float2 textureCoordinate : TEXCOORD0;
};

XENON_SETUP_CAMERA(MonoCamera, camera)

struct RotationBuffer 
{
	float4x4 m_Matrix;
};

cbuffer rotation : register(b0) { RotationBuffer rotation; };

VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = mul(camera.projection, mul(rotation.m_Matrix, float4(input.position, 100.0f)));
	output.textureCoordinate = input.textureCoordinate;
	output.normal = input.normal;

	return output;
}