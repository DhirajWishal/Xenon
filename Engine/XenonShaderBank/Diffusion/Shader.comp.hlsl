// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "../Core/Common.hlsli"

[[vk::combinedImageSampler]]
Texture2D originalImage : register(t0);
[[vk::combinedImageSampler]]
SamplerState originalImageSampler : register(s0);

[[vk::combinedImageSampler]]
Texture2D mipMapImage : register(t1);
[[vk::combinedImageSampler]]
SamplerState mipMapImageSampler : register(s1);

RWTexture2D<float4> resultImage : register(u2);

struct ControlBlock 
{
	uint m_LOD;
};

cbuffer controlBlock : register(b3) { ControlBlock controlBlock; }

float4 getSumOfPixels(uint width, uint height, int2 coordinate, uint distanceX, uint distanceY)
{
	uint endX = coordinate.x + distanceX;
	uint endY = coordinate.y + distanceY;

	float4 color = originalImage[coordinate];
	if(endX < width && endY < height)
	{
		for(int x = coordinate.x; x < endX; x++)
		{
			for(int y = coordinate.y; y < endY; y++)
			{
				color += originalImage[int2(x, y)];
			}
		}

		color /= (distanceX + distanceY);
	}

	return color;
}

float4 downsample(int2 coordinate, uint factor)
{
	float4 gatherValue = float4(0.0f, 0.0f, 0.0f, 0.0f);
	uint counter = 0;
	for(int y = coordinate.y - factor; y < coordinate.y + factor; y += factor)
	{
		for(int x = coordinate.x - factor; x < coordinate.x + factor; x += factor)
		{
			gatherValue += originalImage[int2(x, y)];
			counter++;
		}
	}

	return gatherValue / counter;
}

[numthreads(8, 8, 1)]
void main(uint2 ThreadID : SV_DispatchThreadID)
{
	int2 coordinate = int2(ThreadID.xy);

	uint width;
	uint height;
	originalImage.GetDimensions(width, height);
	float ratio = float(width) / height;

	uint levels = floor(log2(max(width, height))) + 1;
	float4 contribution = originalImage[coordinate] / 2;
	for(uint i = 2; i <= levels; i++)
	{
		uint factor = 1 << i;
		contribution += mipMapImage.mips[i - 2][int2(coordinate.x, coordinate.y)] / factor;
	}

	resultImage[coordinate] = contribution;
}