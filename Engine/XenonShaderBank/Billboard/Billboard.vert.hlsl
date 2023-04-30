// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "Billboard.hlsli"
#include "../Core/VertexInputDefines.hlsli"
#include "../Core/Camera.hlsli"

struct VSInput 
{
	XENON_VERTEX_INPUT_VERTEX_POSITION float2 position : POSITION0;
	XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_0 float2 textureCoordinates : TEXCOORD0;
};

XENON_SETUP_CAMERA(MonoCamera, camera);
XENON_SETUP_TRANSFORM(transform);

VSOutput main(VSInput input)
{
	float4x4 modelView = camera.view;

	modelView[0][0] = 1;
	modelView[0][1] = 0;
	modelView[0][2] = 0;
	
	modelView[1][0] = 0;
	modelView[1][1] = 1;
	modelView[1][2] = 0;
	
	modelView[2][0] = 0;
	modelView[2][1] = 0;
	modelView[2][2] = 1;

	VSOutput output;
	output.position = mul(camera.projection, mul(mul(transform.m_Matrix, modelView), float4(input.position, 0.0f, 2.0f)));
	output.textureCoordinates = input.textureCoordinates;

	return output;
}