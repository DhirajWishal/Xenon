// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "Layer.hpp"
#include "Renderer.hpp"
#include "LayerPass.hpp"

namespace Xenon
{
	Layer::Layer(Renderer& renderer, uint32_t priority)
		: m_Renderer(renderer)
		, m_pCommandRecorder(renderer.getInstance().getFactory()->createCommandRecorder(renderer.getInstance().getBackendDevice(), Backend::CommandRecorderUsage::Graphics, renderer.getCommandRecorder()->getBufferCount()))
		, m_Priority(priority)
	{
	}

	Xenon::Instance& Layer::getInstance() noexcept
	{
		return m_Renderer.getInstance();
	}

	const Xenon::Instance& Layer::getInstance() const noexcept
	{
		return m_Renderer.getInstance();
	}

	void Layer::runPasses(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex) const
	{
		for (const auto& pPass : m_pLayerPasses)
			pPass->onUpdate(pPreviousLayer, imageIndex, frameIndex, m_pCommandRecorder.get());
	}
}