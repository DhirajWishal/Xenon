// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Xenon
{
	namespace Components
	{
		/**
		 * Light source structure.
		 * This is a shader-visible structure which contains information about a single point (or directional) light.
		 */
		struct LightSource final
		{
			glm::vec4 m_Color;
			glm::vec3 m_Position;
			glm::vec3 m_Direction;

			float m_Intensity = 0;	// 0 = No intensity, 1 = Full intensity.
			float m_FieldAngle = 0;	// 0 or 360 = point light.
		};
	}
}