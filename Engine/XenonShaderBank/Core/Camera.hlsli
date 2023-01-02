// Copyright 2022 Dhiraj Wishal
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

#define XENON_SETUP_CAMERA(camera, name)																			\
XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_SCENE, XENON_SCENE_DESCRIPTOR_BINDING_CAMERA)							\
	cbuffer name : register(b0, XENON_DESCRIPTOR_SPACE(XENON_SCENE_DESCRIPTOR_BINDING_CAMERA)) { camera name; }

#endif // CAMERA_HLSLI