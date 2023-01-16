// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Components.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Xenon
{
	namespace Components
	{
		glm::mat4 Transform::computeModelMatrix() const
		{
			return
				glm::translate(glm::mat4(1.0f), m_Position) *
				glm::scale(glm::mat4(1.0f), m_Scale) *
				glm::rotate(glm::mat4(1.0f), m_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::rotate(glm::mat4(1.0f), m_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::rotate(glm::mat4(1.0f), m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		}
	}
}