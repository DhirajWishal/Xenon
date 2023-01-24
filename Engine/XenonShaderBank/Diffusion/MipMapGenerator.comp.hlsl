// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

Texture2D inputImage : register(t0);
RWTexture2D<float4> resultImage : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 globalInvocationID : SV_DispatchThreadID)
{
	float2 coordinate = globalInvocationID.xy;
	resultImage[coordinate] = inputImage[coordinate];
}