// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "Common.hlsli"

#include "../Core/Common.hlsli"

struct FSOutput
{
	[[vk::location(0)]] float4 color : SV_TARGET0;
};

cbuffer controlBlock : register(b0) { LightLUTControlBlock controlBlock; };
RWStructuredBuffer<float> lookUpTable : register(u1);

XENON_SETUP_SCENE_INFORMATION(sceneInformation);
XENON_SETUP_LIGHT_SOURCES(lightSources);

float4 main(VSOutput input) : SV_TARGET0
{
	for(uint i = 0; i < sceneInformation.m_LightSourceCount; i++)
	{
		const LightSource lightSource = lightSources[i];

		const float uniqueID = GetLookUpTableUniqueID(input.transformedPosition.xyz, lightSource.m_Position);
		const float dist = distance(lightSource.m_Position, input.transformedPosition.xyz);

		lookUpTable[FindLookUpTableIndex(uniqueID, i * controlBlock.m_Stride, controlBlock.m_Stride)] = dist;
	}

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}