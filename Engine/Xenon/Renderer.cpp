// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.hpp"
#include "../XenonCore/Logging.hpp"

#include <optick.h>

namespace Xenon
{
	Renderer::Renderer(Instance& instance, Backend::Camera* pCamera, const std::string& title)
		: m_pSwapChain(instance.getFactory()->createSwapchain(instance.getBackendDevice(), title, pCamera->getWidth(), pCamera->getHeight()))
		, m_pCommandRecorder(instance.getFactory()->createCommandRecorder(instance.getBackendDevice(), Backend::CommandRecorderUsage::Graphics, 3))
		, m_pCamera(pCamera)
		, m_Instance(instance)
	{
		m_pCommandSubmitters.reserve(3);
		for (uint8_t i = 0; i < 3; i++)
			m_pCommandSubmitters.emplace_back(instance.getFactory()->createCommandSubmitter(instance.getBackendDevice()));

		m_pSubmitCommandRecorders.emplace_back(m_pCommandRecorder.get());
	}

	bool Renderer::update()
	{
		OPTICK_FRAME("Renderer Update");

		// Return false if we need to close.
		if (!m_IsOpen)
			return false;

		// Wait till all the other tasks have been completed before re-recording the new ones.
		m_TaskGraph.complete();

		// Update the window.
		m_pSwapChain->getWindow()->update();

		// Prepare the swapchain for a new frame.
		const auto imageIndex = m_pSwapChain->prepare();
		const auto frameIndex = m_pCommandRecorder->getCurrentIndex();

		// Wait till all the commands has been executed.
		m_pCommandSubmitters[frameIndex]->wait();

		// Bind the layers.
		Layer* pPreviousLayer = nullptr;
		std::vector<std::shared_ptr<TaskNode>> pTasks;
		for (const auto& pLayer : m_pLayers)
		{
			pTasks.emplace_back(m_TaskGraph.create([pLayer = pLayer.get(), pPreviousLayer, imageIndex, frameIndex] { pLayer->onUpdate(pPreviousLayer, imageIndex, frameIndex); }))->start();
			pPreviousLayer = pLayer.get();
		}

		m_TaskGraph.create([this, pPreviousLayer, frameIndex] { copyToSwapchainAndSubmit(pPreviousLayer, frameIndex); }, pTasks)->start();

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

	void Renderer::copyToSwapchainAndSubmit(Layer* pPreviousLayer, uint32_t frameIndex)
	{
		// Begin the command recorder.
		m_pCommandRecorder->begin();

		// Copy the previous layer's color buffer to the swapchain if we have a layer.
		if (pPreviousLayer)
			m_pCommandRecorder->copy(pPreviousLayer->getColorAttachment(), m_pSwapChain.get());

		// End the command recorder.
		m_pCommandRecorder->end();

		// Submit the commands to the GPU.
		m_pCommandSubmitters[frameIndex]->submit(m_pSubmitCommandRecorders, m_pSwapChain.get());
		// m_pCommandRecorder->submit(m_pSwapChain.get());

		// Present the swapchain.
		m_pSwapChain->present();

		// Select the next command buffer.
		m_pCommandRecorder->next();

		for (const auto& pLayer : m_pLayers)
			pLayer->selectNextCommandBuffer();
	}
}