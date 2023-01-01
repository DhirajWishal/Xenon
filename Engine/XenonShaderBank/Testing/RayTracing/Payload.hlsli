// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#ifndef PAYLOAD_H
#define PAYLOAD_H

struct RayPayload {
	float3 color;
	float distance;
	float3 normal;
	float reflector;
};

#endif // PAYLOAD_H