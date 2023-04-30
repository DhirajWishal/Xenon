// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRenderer.hpp"

#include "../Layers/DirectLightingLayer.hpp"
#include "../Layers/LightLUT.hpp"

namespace Xenon
{
	namespace Experimental
	{
		DefaultRenderer::DefaultRenderer(Instance& instance, uint32_t width, uint32_t height, const std::string& title)
			: Renderer(instance, width, height, title)
		{
			// Setup the GBuffer layers.
			// m_pPositiveXLayer = createLayer<GBufferLayer>(width, height, GBufferFace::PositiveX);
			// m_pNegativeXLayer = createLayer<GBufferLayer>(width, height, GBufferFace::NegativeX);
			// m_pPositiveYLayer = createLayer<GBufferLayer>(width, height, GBufferFace::PositiveY);
			// m_pNegativeYLayer = createLayer<GBufferLayer>(width, height, GBufferFace::NegativeY);
			// m_pPositiveZLayer = createLayer<GBufferLayer>(width, height, GBufferFace::PositiveZ);
			m_pNegativeZLayer = createLayer<GBufferLayer>(width, height, GBufferFace::NegativeZ);

			// Setup the direct lighting layer.
			m_pDirectLightingLayer = createLayer<DirectLightingLayer>(width, height);
			// m_pDirectLightingLayer->setGBuffer(m_pPositiveXLayer);
			// m_pDirectLightingLayer->setGBuffer(m_pNegativeXLayer);
			// m_pDirectLightingLayer->setGBuffer(m_pPositiveYLayer);
			// m_pDirectLightingLayer->setGBuffer(m_pNegativeYLayer);
			// m_pDirectLightingLayer->setGBuffer(m_pPositiveZLayer);
			m_pDirectLightingLayer->setGBuffer(m_pNegativeZLayer);

			// Create the light LUT.
			m_pLightLUT = createLayer<LightLUT>(width, height, 0);
			m_pDirectLightingLayer->setLightLUT(m_pLightLUT);
		}

		void DefaultRenderer::setScene(Scene& scene)
		{
			// Set the scene to the layers.
			// m_pPositiveXLayer->setScene(scene);
			// m_pNegativeXLayer->setScene(scene);
			// m_pPositiveYLayer->setScene(scene);
			// m_pNegativeYLayer->setScene(scene);
			// m_pPositiveZLayer->setScene(scene);
			m_pNegativeZLayer->setScene(scene);
			m_pDirectLightingLayer->setScene(scene);

			m_pLightLUT->setScene(scene);
		}
	}
}