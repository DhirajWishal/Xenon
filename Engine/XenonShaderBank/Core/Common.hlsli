// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#ifndef COMMON_HLSLI
#define COMMON_HLSLI

#define XENON_NAME_CONCAT(first, second)	first##second

#define XENON_SETUP_DESCRIPTOR(set, bindingIndex)	[[vk::binding(bindingIndex, set)]]

#define XENON_DESCRIPTOR_TYPE_USER_DEFINED	0
#define XENON_DESCRIPTOR_TYPE_MATERIAL		1
#define XENON_DESCRIPTOR_TYPE_PER_GEOMETRY	2
#define XENON_DESCRIPTOR_TYPE_SCENE			3

#define XENON_PER_GEOMETRY_DESCRIPTOR_BINDING_TRANSFORM		0

#define XENON_SCENE_DESCRIPTOR_BINDING_SCENE_INFORMATION		0
#define XENON_SCENE_DESCRIPTOR_BINDING_CAMERA					1
#define XENON_SCENE_DESCRIPTOR_BINDING_LIGHT_SOURCES			2
#define XENON_SCENE_DESCRIPTOR_BINDING_ACCELERATION_STRUCTURE	3
#define XENON_SCENE_DESCRIPTOR_BINDING_RENDER_TARGET			4

#define XENON_DESCRIPTOR_SPACE(index)		XENON_NAME_CONCAT(space, index)

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

/**
 * Transform structure.
 * This contains the model matrix of a geometry.
 */
struct Transform
{
	float4x4 m_Matrix;
};

#define XENON_SETUP_TRANSFORM(name)																					\
	XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_PER_GEOMETRY, XENON_PER_GEOMETRY_DESCRIPTOR_BINDING_TRANSFORM)		\
	cbuffer name : register(b0, XENON_DESCRIPTOR_SPACE(XENON_DESCRIPTOR_TYPE_PER_GEOMETRY)) { Transform name; }

/**
 * Light source structure.
 * This structure contains information about a single light source (point or directional).
 */
struct LightSource
{
	float4 m_Color;
	float3 m_Position;
	float3 m_Direction;

	float m_Intensity;
	float m_FieldAngle;
};

/**
 * Scene information structure.
 * This structure contains information about the current scene.
 */
struct SceneInformation
{
	uint m_LightSourceCount;
};

#define XENON_SETUP_SCENE_INFORMATION(name)																		\
	XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_SCENE, XENON_SCENE_DESCRIPTOR_BINDING_SCENE_INFORMATION)		\
	cbuffer name : register(b0, XENON_DESCRIPTOR_SPACE(XENON_DESCRIPTOR_TYPE_SCENE)) { SceneInformation name; }

#define XENON_SETUP_CAMERA(camera, name)																		\
	XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_SCENE, XENON_SCENE_DESCRIPTOR_BINDING_CAMERA)					\
	cbuffer name : register(b1, XENON_DESCRIPTOR_SPACE(XENON_DESCRIPTOR_TYPE_SCENE)) { camera name; }

#define XENON_SETUP_LIGHT_SOURCES(name)																		\
	XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_SCENE, XENON_SCENE_DESCRIPTOR_BINDING_LIGHT_SOURCES)		\
	ConstantBuffer<LightSource> name : register(b2, XENON_DESCRIPTOR_SPACE(XENON_DESCRIPTOR_TYPE_SCENE))

#define XENON_SETUP_ACCELERATION_STRUCTURE(name)																\
	XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_SCENE, XENON_SCENE_DESCRIPTOR_BINDING_ACCELERATION_STRUCTURE) 	\
	RaytracingAccelerationStructure name : register(t3, XENON_DESCRIPTOR_SPACE(XENON_DESCRIPTOR_TYPE_SCENE))

#define XENON_SETUP_RENDER_TARGET_IMAGE(type, name)																\
	XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_SCENE, XENON_SCENE_DESCRIPTOR_BINDING_RENDER_TARGET)			\
	RWTexture2D<type> name : register(u4, XENON_DESCRIPTOR_SPACE(XENON_DESCRIPTOR_TYPE_SCENE))	

#endif // COMMON_HLSLI