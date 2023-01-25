// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GBufferLayer.hpp"
#include "../Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Xenon
{
	namespace Experimental
	{
		GBufferLayer::GBufferLayer(Renderer& renderer, Backend::Camera* pCamera, GBufferFace face /*= GBufferFace::Front*/, uint32_t priority /*= 0*/)
			: RasterizingLayer(renderer, priority, pCamera, Backend::AttachmentType::Color | Backend::AttachmentType::Normal | Backend::AttachmentType::Depth)
			, m_Face(face)
		{
			switch (face)
			{
			case Xenon::Experimental::GBufferFace::PositiveX:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case Xenon::Experimental::GBufferFace::NegativeX:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case Xenon::Experimental::GBufferFace::PositiveY:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				break;

			case Xenon::Experimental::GBufferFace::NegativeY:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				break;

			// case Xenon::Experimental::GBufferFace::PositiveZ:
			// 	m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			// 	break;

			case Xenon::Experimental::GBufferFace::NegativeZ:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;

			default:
				break;
			}
		}

		void GBufferLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{

		}
	}
}