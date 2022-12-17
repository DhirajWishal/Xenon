// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LinuxWindow.hpp"

namespace Xenon
{
	namespace Platform
	{
		LinuxWindow::LinuxWindow(const std::string& title, uint32_t width, uint32_t height)
		: Window(title, width, height)
		{

		}

		LinuxWindow::~LinuxWindow()
		{

		}

		void LinuxWindow::update()
		{

		}

		bool LinuxWindow::isOpen() const
		{
			return m_IsOpen;
		}
	}
}