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
	}

	Renderer::~Renderer()
	{
		m_bShouldRun = false;
		m_WorkerSynchronization.notify_one();
	}

	bool Renderer::update()
	{
		m_WorkerSynchronization.notify_one();
		return m_pSwapChain->getWindow()->isOpen();
	}

	void Renderer::wait()
	{
		auto locker = std::unique_lock(m_SynchronizationMutex);
		m_ParentSynchronization.wait(locker);
	}

	void Renderer::worker()
	{
		auto locker = std::unique_lock(m_SynchronizationMutex);

		do
		{
			// Wait till the user wants us to progress.
			m_WorkerSynchronization.wait(locker);

			// Return if we have to.
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

			// End the command recorder.
			m_pCommandRecorder->end();

			// Submit the commands to the GPU.
			m_pCommandRecorder->submit(m_pSwapChain.get());

			// Present the swapchain.
			m_pSwapChain->present();

			// Update the window.
			m_pSwapChain->getWindow()->update();

			// Select the next command buffer.
			m_pCommandRecorder->next();

			// Notify the parent that the update has been completed.
			m_ParentSynchronization.notify_one();
		} while (true);
	}
}