// Copyright 2022-2023 Dhiraj Wishal
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

float4x4 translate(float4x4 mat, float3 position)
{
	mat[0][3] = position.x;
    mat[1][3] = position.y;
    mat[2][3] = position.z;

    return mat;
}

float4x4 scale(float4x4 mat, float3 scale)
{
	mat[0][0] *= scale.x; mat[1][0] *= scale.y; mat[2][0] *= scale.z;
    mat[0][1] *= scale.x; mat[1][1] *= scale.y; mat[2][1] *= scale.z;
    mat[0][2] *= scale.x; mat[1][2] *= scale.y; mat[2][2] *= scale.z;
    mat[0][3] *= scale.x; mat[1][3] *= scale.y; mat[2][3] *= scale.z;

    return mat;
}

VSOutput main(VSInput input)
{
	float4x4 modelMatrix = translate(GetIdentityMatrix(), transform.m_Position);
	modelMatrix = scale(modelMatrix, transform.m_Scale);

	VSOutput output;
	output.position = mul(camera.projection * camera.view * modelMatrix, float4(input.position, 1.0f, 100.0f));
	output.textureCoordinates = input.textureCoordinates;

	return output;
}