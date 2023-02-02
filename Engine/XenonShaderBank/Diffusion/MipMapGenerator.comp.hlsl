// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "../Core/Common.hlsli"

[[vk::combinedImageSampler]]
Texture2D originalImage : register(t0);
[[vk::combinedImageSampler]]
SamplerState originalImageSampler : register(s0);

RWTexture3D<float4> mipMapImage : register(u1);

// float4 downsample(int2 coordinate, uint factor)
// {
// 	if(factor == 0)
// 		return originalImage[coordinate];

// 	float4 gatherValue = float4(0.0f, 0.0f, 0.0f, 0.0f);
// 	for(int y = coordinate.y - factor; y < coordinate.y + factor; y += factor)
// 	{
// 		for(int x = coordinate.x - factor; x < coordinate.x + factor; x += factor)
// 		{
// 			gatherValue += originalImage[int2(x, y)];
// 		}
// 	}

// 	return gatherValue;
// }

float4 downsample(int2 coordinate, uint factor)
{
	if(factor == 0)
		return originalImage[coordinate];

	float4 gatherValue = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for(int y = coordinate.y - factor; y < coordinate.y + factor; y++)
	{
		for(int x = coordinate.x - factor; x < coordinate.x + factor; x++)
		{
			gatherValue += originalImage[int2(x, y)];
		}
	}

	return gatherValue;
}

[numthreads(8, 8, 1)]
void main(uint2 ThreadID : SV_DispatchThreadID)
{
	uint width;
	uint height;
	originalImage.GetDimensions(width, height);
	uint levels = floor(log2(max(width, height))) + 1;

	int2 coordinate = int2(ThreadID.xy);
	for(uint i = 0; i < levels; i++)
	{
		uint factor = 1 << i;
		mipMapImage[int3(coordinate.x, coordinate.y, i)] = downsample(coordinate, i * factor);
	}
}