// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.hpp"
#include "../XenonCore/Logging.hpp"

#include <optick.h>

namespace Xenon
{
	Renderer::Renderer(Instance& instance, uint32_t width, uint32_t height, const std::string& title)
		: m_pSwapChain(instance.getFactory()->createSwapchain(instance.getBackendDevice(), title, width, height))
		, m_pCommandRecorder(instance.getFactory()->createCommandRecorder(instance.getBackendDevice(), Backend::CommandRecorderUsage::Graphics, 3))
		, m_Instance(instance)
	{
		m_pCommandSubmitters.reserve(3);
		for (uint8_t i = 0; i < 3; i++)
			m_pCommandSubmitters.emplace_back(instance.getFactory()->createCommandSubmitter(instance.getBackendDevice()));
	}

	bool Renderer::update()
	{
		OPTICK_FRAME("Renderer Update");

		// Return false if we need to close.
		if (!m_IsOpen)
			return false;

		// Update the window.
		m_pSwapChain->getWindow()->update();

		// Wait till all the commands has been executed.
		const auto frameIndex = m_pCommandRecorder->getCurrentIndex();
		m_pCommandSubmitters[frameIndex]->wait();

		// Prepare the swapchain for a new frame.
		const auto imageIndex = m_pSwapChain->prepare();

		// Bind the layers.
		Layer* pPreviousLayer = nullptr;
		m_CountingFence.reset(m_pLayers.size() + 1);
		for (const auto& pLayer : m_pLayers)
		{
			pLayer->onPreUpdate();
			GetJobSystem().insert([this, pLayer = pLayer.get(), pPreviousLayer, imageIndex, frameIndex] { updateLayer(pLayer, pPreviousLayer, imageIndex, frameIndex); });
			pPreviousLayer = pLayer.get();
		}

		// Copy the previous layer to the swapchain.
		GetJobSystem().insert([this, pPreviousLayer] { copyToSwapchainAndSubmit(pPreviousLayer); });

		// Wait till all the required jobs are done.
		m_CountingFence.wait();

		// Submit the commands to the GPU.
		m_pCommandSubmitters[m_pCommandRecorder->getCurrentIndex()]->submit(m_pSubmitCommandRecorders, m_pSwapChain.get());

		// Present the swapchain.
		m_pSwapChain->present();

		// Select the next command buffer.
		m_pCommandRecorder->next();

		// Do the same for the layers.
		for (const auto& pLayer : m_pLayers)
			pLayer->selectNextCommandBuffer();

		return m_pSwapChain->getWindow()->isOpen();
	}

	void Renderer::cleanup()
	{
		m_Instance.getBackendDevice()->waitIdle();
		m_pLayers.clear();
	}

	void Renderer::close()
	{
		m_IsOpen = false;
	}

	void Renderer::insertLayer(std::unique_ptr<Layer>&& pLayer)
	{
		const auto itr = XENON_RANGES(upper_bound, m_pLayers, pLayer, [](const auto& lhs, const auto& rhs) { return lhs->getPriority() < rhs->getPriority(); });
		m_pLayers.emplace(itr, std::move(pLayer));

		// Update the command recorders.
		updateSubmitCommandRecorders();
	}

	void Renderer::updateSubmitCommandRecorders()
	{
		m_pSubmitCommandRecorders.clear();

		uint32_t previousPriority = -1;
		for (const auto& pLayer : m_pLayers)
		{
			auto pCommandRecorder = pLayer->getCommandRecorder();
			if (previousPriority == pLayer->getPriority())
				m_pSubmitCommandRecorders.back().emplace_back(pCommandRecorder);

			else
				m_pSubmitCommandRecorders.emplace_back().emplace_back(pCommandRecorder);

			previousPriority = pLayer->getPriority();
		}

		m_pSubmitCommandRecorders.emplace_back().emplace_back(m_pCommandRecorder.get());
	}

	void Renderer::updateLayer(Layer* pLayer, Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
	{
		OPTICK_EVENT();

		// Update the layer.
		pLayer->onUpdate(pPreviousLayer, imageIndex, frameIndex);

		// Notify that we're done.
		m_CountingFence.arrive();
	}

	void Renderer::copyToSwapchainAndSubmit(Layer* pPreviousLayer)
	{
		OPTICK_EVENT();

		// Begin the command recorder.
		m_pCommandRecorder->begin();

		// Copy the previous layer's color buffer to the swapchain if we have a layer.
		if (pPreviousLayer)
			m_pCommandRecorder->copy(pPreviousLayer->getColorAttachment(), m_pSwapChain.get());

		// End the command recorder.
		m_pCommandRecorder->end();

		// Notify that we're done.
		m_CountingFence.arrive();
	}
}