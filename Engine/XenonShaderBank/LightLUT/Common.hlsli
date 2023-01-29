// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#ifndef LIGHT_LUT_COMMON_HLSLI
#define LIGHT_LUT_COMMON_HLSLI

#ifndef LIGHT_LUT_HIDE_OUTPUT
struct VSOutput 
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float4 transformedPosition : POSITION0;
	[[vk::location(1)]] uint vertexID : POSITION1;
};

#endif

struct LightLUTControlBlock 
{
	uint m_Stride;
};

uint FindLookUpTableIndex(float uniqueID, uint start = 0, uint size = 1000)
{
	const uint hash = asuint(uniqueID);
	for(uint i = 0; (i < 32) && (size > 0); i++)
	{
		size /= 2;
		start += bool(hash & (1 << i)) * size;
	}

	return start;
}

#endif // LIGHT_LUT_COMMON_HLSLI