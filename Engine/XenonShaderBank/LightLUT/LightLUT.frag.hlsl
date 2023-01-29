// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Common.hlsli"

#include "../Core/Common.hlsli"

struct FSOutput
{
	[[vk::location(0)]] float4 color : SV_TARGET0;
};

cbuffer controlBlock : register(b0) { LightLUTControlBlock controlBlock; };
RWStructuredBuffer<float2> lookUpTable : register(u1);

XENON_SETUP_SCENE_INFORMATION(sceneInformation);
XENON_SETUP_LIGHT_SOURCES(lightSources);

float4 main(VSOutput input) : SV_TARGET0
{
	for(uint i = 0; i < sceneInformation.m_LightSourceCount; i++)
	{
		const LightSource lightSource = lightSources[i];

		const float3 delta = input.transformedPosition.xyz - lightSource.m_Position;
		const float pitch = tan(delta.z / delta.x);
		const float yaw = tan(delta.y / delta.x);

		const float uniqueID = (pitch * 1000) + yaw;
		const float dist = distance(lightSource.m_Position, input.transformedPosition.xyz);

		lookUpTable[FindLookUpTableIndex(uniqueID, i * controlBlock.m_Stride, controlBlock.m_Stride)] = float2(uniqueID, dist);
	}

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}