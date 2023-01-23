// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

/**
 * We have to separately and explicitly define the alignment of the glm::vec3 structure when building a shader-visible buffer.
 * This is because in C++ the glm::vec3 alignment is not equal to the HLSL float3 alignment. It actually requires the alignment to be
 * the same as glm::vec4.
 * 
 * So when defining a 3 component shader-visible vector (stand-along or in a struct) make sure to add this to avoid any unwanted errors.
 * Usage:
 *	XENON_HLSL_VEC3_ALIGNMENT glm::vec3 variable = ...;
 */
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

			/**
			 * Compute the model matrix from the transform data.
			 *
			 * @return The model matrix which will be passed to the shader.
			 */
			[[nodiscard]] glm::mat4 computeModelMatrix() const;
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