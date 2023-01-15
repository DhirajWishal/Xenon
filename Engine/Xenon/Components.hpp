// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define XENON_HLSL_VEC3_ALIGNMENT alignas(sizeof(glm::vec4))

namespace Xenon
{
	namespace Components
	{
		/**
		 * Transform structure.
		 * This is a shader-visible structure which contains information about a single transform used to position a geometry in 3D space.
		 */
		struct Transform final
		{
			XENON_HLSL_VEC3_ALIGNMENT glm::vec3 m_Position = glm::vec3(0);
			XENON_HLSL_VEC3_ALIGNMENT glm::vec3 m_Rotation = glm::vec3(0);
			XENON_HLSL_VEC3_ALIGNMENT glm::vec3 m_Scale = glm::vec3(1.0);
		};

		/**
		 * Light source structure.
		 * This is a shader-visible structure which contains information about a single point (or directional) light.
		 */
		struct LightSource final
		{
			glm::vec4 m_Color;
			XENON_HLSL_VEC3_ALIGNMENT glm::vec3 m_Position;
			XENON_HLSL_VEC3_ALIGNMENT glm::vec3 m_Direction;

			float m_Intensity = 0;	// 0 = No intensity, 1 = Full intensity.
			float m_FieldAngle = 0;	// 0 or 360 = point light.
		};
	}
}