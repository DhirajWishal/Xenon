// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.hpp"

namespace Xenon
{
	Renderer::Renderer(Instance& instance, Backend::Camera* pCamera, const std::string& title)
		: m_pSwapChain(instance.getFactory()->createSwapchain(instance.getBackendDevice(), title, pCamera->getWidth(), pCamera->getHeight()))
	{
	}

	bool Renderer::update()
	{
		m_pSwapChain->getWindow()->update();
		return m_pSwapChain->getWindow()->isOpen();
	}
}