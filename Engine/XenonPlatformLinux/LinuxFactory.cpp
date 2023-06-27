// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LinuxFactory.hpp"
#include "LinuxWindow.hpp"

#include "../XenonCore/Logging.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace Xenon
{
	namespace Platform
	{
		LinuxFactory::LinuxFactory()
		{
			// Try and initialize SDL.
			if (SDL_Init(SDL_INIT_VIDEO) != 0)
				XENON_LOG_FATAL("Failed to initialize SDL! {}", SDL_GetError());

			else
				XENON_LOG_INFORMATION("Successfully initialized SDL.");

			// Load the Vulkan library to SDL.
			if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
				XENON_LOG_FATAL("Failed to load the Vulkan library in SDL! {}", SDL_GetError());
		}

		LinuxFactory::~LinuxFactory()
		{
			// Unload the Vulkan library and quit SDL.
			SDL_Vulkan_UnloadLibrary();
			SDL_Quit();
		}

		std::unique_ptr<Xenon::Platform::Window> LinuxFactory::createWindow(const std::string& title, uint32_t width, uint32_t height)
		{
			return std::make_unique<Xenon::Platform::LinuxWindow>(title, width, height);
		}
	}
}