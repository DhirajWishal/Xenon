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
		m_Synchronization.notify_one();
	}

	bool Renderer::update()
	{
		m_Synchronization.notify_one();
		return m_pSwapChain->getWindow()->isOpen();
	}

	void Renderer::worker()
	{
		auto locker = std::unique_lock(m_SynchronizationMutex);

		do
		{
			// Wait till the user wants us to progress.
			m_Synchronization.wait(locker, [this] { return !m_bShouldRun; });

			// Return if we have to.
			if (m_bShouldRun == false)
				break;

			// Begin the command recorder.
			m_pCommandRecorder->begin();

			// End the command recorder.
			m_pCommandRecorder->end();

			// Submit the commands to the GPU.
			m_pCommandRecorder->submit(m_pSwapChain.get());

			// Present the swapchain.
			m_pSwapChain->present();

			// Update the window.
			m_pSwapChain->getWindow()->update();

		} while (true);
	}
}