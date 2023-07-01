// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Swapchain.hpp"

#include "../XenonPlatform/IFactory.hpp"

namespace Xenon
{
	namespace Backend
	{
		Swapchain::Swapchain(XENON_MAYBE_UNUSED const Device* pDevice, const std::string& title, uint32_t width, uint32_t height)
			: m_pWindow(Platform::IFactory::Get()->createWindow(title, width, height))
		{
		}

		void Swapchain::incrementFrame()
		{
			m_FrameIndex = ++m_FrameIndex % m_FrameCount;
		}
	}
}