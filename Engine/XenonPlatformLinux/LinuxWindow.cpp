// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LinuxWindow.hpp"

#include "../XenonCore/Logging.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <optick.h>

namespace Xenon
{
	namespace Platform
	{
		LinuxWindow::LinuxWindow(const std::string& title, uint32_t width, uint32_t height)
			: Window(title, width, height)
		{
			// Create the window.
			m_pWindow = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_FULLSCREEN);

			// Check if the window was created properly.
			if (!m_pWindow)
			{
				XENON_LOG_FATAL("Failed to create the window!");
				return;
			}

			// Set this class as user data.
			SDL_SetWindowData(m_pWindow, "this", this);
		}

		LinuxWindow::~LinuxWindow()
		{
			while (isOpen()) update();
		}

		void LinuxWindow::update()
		{
			OPTICK_EVENT();

			m_Keyboard.m_Character = 0;
			m_Mouse.m_VScroll = 0.0f;
			m_Mouse.m_HScroll = 0.0f;

			SDL_Event events;
			while (SDL_PollEvent(&events))
			{
				// Handle the events.
			}
		}

		bool LinuxWindow::isOpen() const
		{
			return m_IsOpen;
		}
	}
}