// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Swapchain.hpp"

#include "../XenonPlatform/IFactory.hpp"

namespace Xenon
{
	namespace Backend
	{
		Swapchain::Swapchain([[maybe_unused]] const Device* pDevice, const std::string& title, uint32_t width, uint32_t height)
			: m_pWindow(Platform::IFactory::Get()->createWindow(title, width, height))
		{
		}

		void Swapchain::incrementFrame()
		{
			m_FrameIndex = ++m_FrameIndex % m_FrameCount;
		}
	}
}