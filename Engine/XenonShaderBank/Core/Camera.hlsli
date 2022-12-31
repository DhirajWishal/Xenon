// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#ifndef CAMERA_HLSLI
#define CAMERA_HLSLI

/**
 * Mono camera structure.
 * The mono camera contains a single lense (view and projection matrix).
 */
struct MonoCamera 
{
	float4x4 view;
	float4x4 projection;
};

#define XENON_SETUP_CAMERA(camera, name)	cbuffer name : register(b0, space3) { camera name; }

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

#endif // CAMERA_HLSLI