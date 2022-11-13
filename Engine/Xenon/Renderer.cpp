// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.hpp"

namespace Xenon
{
	Renderer::Renderer(Instance& instance, Backend::Camera* pCamera, const std::string& title)
		: m_Worker([this] { worker(); })
		, m_pSwapChain(instance.getFactory()->createSwapchain(instance.getBackendDevice(), title, pCamera->getWidth(), pCamera->getHeight()))
		, m_pCommandRecorder(instance.getFactory()->createCommandRecorder(instance.getBackendDevice(), Backend::CommandRecorderUsage::Graphics, 3))
		, m_pCamera(pCamera)
	{
		m_Latch.count_down();
	}

	Renderer::~Renderer()
	{
		m_bShouldRun = false;
		m_WorkerSynchronization.notify_one();

		m_Worker.join();
	}

	bool Renderer::update()
	{
		m_WorkerSynchronization.notify_one();

		m_pSwapChain->getWindow()->update();
		return m_pSwapChain->getWindow()->isOpen();
	}

	void Renderer::worker()
	{
		m_Latch.wait();
		auto locker = std::unique_lock(m_SynchronizationMutex);

		do
		{
			// Wait till notified.
			m_WorkerSynchronization.wait(locker);

			// Return if requested.
			if (m_bShouldRun == false)
				break;

			// Prepare the swapchain for a new frame.
			[[maybe_unused]] const auto imageIndex = m_pSwapChain->prepare();

			// Begin the command recorder.
			m_pCommandRecorder->begin();

			// Bind the layers.
			Layer* pPreviousLayer = nullptr;
			for (const auto& pLayer : m_pLayers)
			{
				pLayer->bind(pPreviousLayer, m_pCommandRecorder.get());
				pPreviousLayer = pLayer.get();
			}

			// Copy the previous layer's color buffer to the swapchain if we have a layer.
			if (pPreviousLayer)
				m_pCommandRecorder->copy(pPreviousLayer->getColorAttachment(), m_pSwapChain.get());

			// End the command recorder.
			m_pCommandRecorder->end();

			// Submit the commands to the GPU.
			m_pCommandRecorder->submit(m_pSwapChain.get());

			// Present the swapchain.
			m_pSwapChain->present();

			// Select the next command buffer.
			m_pCommandRecorder->next();
		} while (true);
	}
}