// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.hpp"
#include "../XenonCore/Logging.hpp"

namespace /* anonymous */
{
	std::vector<Xenon::Backend::DescriptorBindingInfo> GetCameraBindingInfo()
	{
		std::vector<Xenon::Backend::DescriptorBindingInfo> bindingInfo;
		auto& info = bindingInfo.emplace_back();
		info.m_ApplicableShaders = Xenon::Backend::ShaderType::Vertex;
		info.m_Type = Xenon::Backend::ResourceType::UniformBuffer;

		return bindingInfo;
	}
}

namespace Xenon
{
	Renderer::Renderer(Instance& instance, Backend::Camera* pCamera, const std::string& title)
		: m_Worker([this] { worker(); })
		, m_pSwapChain(instance.getFactory()->createSwapchain(instance.getBackendDevice(), title, pCamera->getWidth(), pCamera->getHeight()))
		, m_pCommandRecorder(instance.getFactory()->createCommandRecorder(instance.getBackendDevice(), Backend::CommandRecorderUsage::Graphics, 3))
		, m_pCamera(pCamera)
		, m_Instance(instance)
	{
		m_Latch.count_down();
		m_pCameraDescriptors.resize(3);
		for (auto& pDescriptor : m_pCameraDescriptors)
		{
			pDescriptor = instance.getFactory()->createDescriptor(instance.getBackendDevice(), GetCameraBindingInfo(), Backend::DescriptorType::Camera);
			pDescriptor->attach(0, m_pCamera->getViewports().front().m_pUniformBuffer);
		}
	}

	Renderer::~Renderer()
	{
		XENON_LOG_TRACE("Terminating the renderer.");

		m_bShouldRun = false;
		m_WorkerSynchronization.notify_one();

		XENON_LOG_TRACE("Waiting till the worker joins...");
		m_Worker.join();
		XENON_LOG_TRACE("The worker joined!");
	}

	bool Renderer::update()
	{
		m_pSwapChain->getWindow()->update();
		m_WorkerSynchronization.notify_one();
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

			// Wait till all the other tasks have been completed before submitting the commands.
			m_TaskGraph.complete();

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