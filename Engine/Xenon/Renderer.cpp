// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.hpp"

namespace Xenon
{
	Renderer::Renderer(Instance& instance, const std::string& title, uint32_t width, uint32_t height)
		: m_pSwapChain(instance.getFactory()->createSwapchain(instance.getBackendDevice(), title, width, height))
	{
	}

	bool Renderer::update()
	{
		m_pSwapChain->getWindow()->update();
		return m_pSwapChain->getWindow()->isOpen();
	}
}