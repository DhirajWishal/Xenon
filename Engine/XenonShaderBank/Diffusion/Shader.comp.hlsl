// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "../Core/Common.hlsli"

Texture2D originalImage : register(t0);
Texture2D mipMapImage : register(t1);
RWTexture2D<float4> resultImage : register(u2);

[numthreads(16, 16, 1)]
void main(uint3 GlobalInvocationID : SV_DispatchThreadID)
{

}