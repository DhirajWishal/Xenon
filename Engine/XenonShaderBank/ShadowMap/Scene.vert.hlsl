// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "../Core/VertexInputDefines.hlsli"
#include "../Core/Camera.hlsli"
#include "SceneCommon.hlsli"

struct VSInput 
{
	XENON_VERTEX_INPUT_VERTEX_POSITION float3 position : POSITION0;
	XENON_VERTEX_INPUT_VERTEX_NORMAL float3 normal : NORMAL0;
	XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_0 float2 textureCoordinates : TEXCOORD0;
};

XENON_SETUP_CAMERA(MonoCamera, camera);

XENON_SETUP_SCENE_INFORMATION(sceneInformation);
XENON_SETUP_LIGHT_SOURCES(lightSources);

struct ShadowMapCamera
{
	float4x4 m_View;
	float4x4 m_Projection;
};

XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_MATERIAL, 2)
cbuffer shadowCamera : register(b2) { ShadowMapCamera shadowCamera; };

static const float4x4 biasMat = float4x4(
	0.5, 0.0, 0.0, 0.5,
	0.0, 0.5, 0.0, 0.5,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0 );

VSOutput main(VSInput input)
{
	const float4x4 identityMatrix = GetIdentityMatrix(); 

	VSOutput output;
	output.position = mul(camera.projection, mul(camera.view, mul(identityMatrix, float4(input.position, 100.0f))));
	output.textureCoordinates = input.textureCoordinates;
	output.normal = input.normal;

	for(uint i = 0; i < sceneInformation.m_LightSourceCount; i++)
	{
		LightSource lightSource = lightSources[i];

		float4 pos = mul(identityMatrix, float4(input.position, 1.0));
		output.normal = mul((float3x3)identityMatrix, input.normal);
    	output.lightVector = normalize(lightSource.m_Position.xyz - input.position);
    	output.viewVector = -pos.xyz;

		output.shadowCoordinate = mul(biasMat, mul(mul(shadowCamera.m_View, shadowCamera.m_Projection), mul(identityMatrix, float4(input.position, 1.0))));
	}

	return output;
}