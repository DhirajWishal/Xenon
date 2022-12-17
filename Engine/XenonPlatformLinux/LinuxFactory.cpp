// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LinuxFactory.hpp"
#include "LinuxWindow.hpp"

namespace Xenon
{
	namespace Platform
	{
		std::unique_ptr<Window> LinuxFactory::createWindow(const std::string &title, uint32_t width, uint32_t height)
		{
			return std::make_unique<LinuxWindow>(title, width, height);
		}
	}
}