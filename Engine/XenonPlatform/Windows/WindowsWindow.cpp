// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "WindowsWindow.hpp"

namespace Xenon
{
	namespace Platform
	{
		WindowsWindow::WindowsWindow(const std::string& title, uint32_t width, uint32_t height)
			: Window(title, width, height)
		{
		}
	}
}