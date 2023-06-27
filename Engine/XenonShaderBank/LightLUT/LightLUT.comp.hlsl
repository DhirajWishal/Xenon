// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "../Core/Common.hlsli"

static const float PI = 3.14159265f;

struct Vertex 
{
	float3 m_Position;
	float3 m_Normal;
	float2 m_TextureCoordintes;
};

// StructuredBuffer<Vertex> verteices : register(u0);
// StructuredBuffer<uint> indices : register(u1);

// RWTexture2D<float> lookUpTable : register(t2);

[numthreads(3, 1, 1)]
void main(uint2 ThreadID : SV_DispatchThreadID)
{
	// Vertex vertex = verteices[indices[int(ThreadID.x)]];
	// const float3 origin = float3(0.0f, 0.0f, 0.0f);

	// float3 delta = origin - vertex.m_Position;
	// float yaw = atan2(delta.z, delta.x);
	// float pitch = atan2(sqrt(delta.z * delta.z + delta.x * delta.x), delta.y) + PI;
	// float distance = distance(origin, vertex.m_Position);
}