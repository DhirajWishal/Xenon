// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRenderer.hpp"

#include "../Layers/GBufferLayer.hpp"

namespace Xenon
{
	namespace Experimental
	{
		DefaultRenderer::DefaultRenderer(Instance& instance, Backend::Camera* pCamera, const std::string& title)
			: Renderer(instance, pCamera, title)
		{
			// Setup the GBuffer layers.
			m_pPositiveXLayer = createLayer<GBufferLayer>(pCamera, GBufferFace::PositiveX);
			m_pNegativeXLayer = createLayer<GBufferLayer>(pCamera, GBufferFace::NegativeX);
			m_pPositiveYLayer = createLayer<GBufferLayer>(pCamera, GBufferFace::PositiveY);
			m_pNegativeYLayer = createLayer<GBufferLayer>(pCamera, GBufferFace::NegativeY);
			m_pPositiveZLayer = createLayer<GBufferLayer>(pCamera, GBufferFace::PositiveZ);
			m_pNegativeZLayer = createLayer<GBufferLayer>(pCamera, GBufferFace::NegativeZ);
		}

		void DefaultRenderer::setScene(Scene& scene)
		{
			// Set the scene to the GBuffer layers.
			m_pPositiveXLayer->setScene(scene);
			m_pNegativeXLayer->setScene(scene);
			m_pPositiveYLayer->setScene(scene);
			m_pNegativeYLayer->setScene(scene);
			m_pPositiveZLayer->setScene(scene);
			m_pNegativeZLayer->setScene(scene);
		}
	}
}