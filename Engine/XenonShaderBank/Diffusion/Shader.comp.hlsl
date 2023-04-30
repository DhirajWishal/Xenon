// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "../Core/Common.hlsli"

[[vk::combinedImageSampler]]
Texture2D originalImage : register(t0);
[[vk::combinedImageSampler]]
SamplerState originalImageSampler : register(s0);

RWTexture2D<float4> resultImage : register(u1);

[numthreads(8, 8, 1)]
void main(uint2 ThreadID : SV_DispatchThreadID)
{
	int2 coordinate = int2(ThreadID.xy);
	resultImage[coordinate] = originalImage[coordinate];
}