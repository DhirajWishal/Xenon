// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "WindowsFactory.hpp"
#include "WindowsWindow.hpp"

namespace Xenon
{
	namespace Platform
	{
		std::unique_ptr<Xenon::Platform::Window> WindowsFactory::createWindow(const std::string& title, uint32_t width, uint32_t height)
		{
			return std::make_unique<Xenon::Platform::WindowsWindow>(title, width, height);
		}
	}
}