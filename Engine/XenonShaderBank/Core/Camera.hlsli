// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#ifndef CAMERA_HLSLI
#define CAMERA_HLSLI

#include "Common.hlsli"

/**
 * Mono camera structure.
 * The mono camera contains a single lens (view and projection matrix).
 */
struct MonoCamera 
{
	float4x4 view;
	float4x4 projection;
};

#endif // CAMERA_HLSLI