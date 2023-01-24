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

[numthreads(16, 16, 1)]
void main(uint3 GlobalInvocationID : SV_DispatchThreadID)
{
	int2 coordinate = int2(GlobalInvocationID.xy);

	uint width;
	uint height;
	originalImage.GetDimensions(width, height);

	uint levels = floor(log2(max(width, height)));
	float4 contribution = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for(uint i = 0; i < levels; i++)
		contribution += mipMapImage.mips[i][coordinate] / int(1 << i);

	resultImage[coordinate] = originalImage[coordinate] + contribution;
}