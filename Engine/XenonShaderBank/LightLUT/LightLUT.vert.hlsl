// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "Common.hlsli"

#include "../Core/VertexInputDefines.hlsli"
#include "../Core/Camera.hlsli"

struct VSInput 
{
	XENON_VERTEX_INPUT_VERTEX_POSITION float3 position : POSITION0;
	uint vertexID : SV_VertexID;
};

XENON_SETUP_CAMERA(MonoCamera, camera)

VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = float4(0.0f, 0.0f, 0.5f, 1.0f);	// This is constant. We want the fragment shader to sample every vetex.
	output.transformedPosition = mul(camera.projection, mul(camera.view, float4(input.position, 100.0f)));
	output.vertexID = input.vertexID;

	return output;
}