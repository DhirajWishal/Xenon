// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_HLSLI
#define COMMON_HLSLI

#define XENON_SETUP_DESCRIPTOR(set, bindingIndex)	[[vk::binding(bindingIndex, set)]]

#define XENON_DESCRIPTOR_TYPE_USER_DEFINED	0
#define XENON_DESCRIPTOR_TYPE_MATERIAL		1
#define XENON_DESCRIPTOR_TYPE_CAMERA		2

#define XENON_NAME_CONCAT(first, second)	first##second

float4x4 GetIdentityMatrix()
{
	float4x4 identity =
	{
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 }
	};

	return identity;
}

#endif // COMMON_HLSLI