// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "../../Core/Common.hlsli"
#include "../../Core/Camera.hlsli"

struct RayPayload
{
	float3 color;
	float distance;
	float3 normal;
	float reflector;
};

struct UBO
{
	float4x4 viewInverse;
	float4x4 projInverse;
	float4 lightPos;
	int vertexSize;
};

#endif // PAYLOAD_H