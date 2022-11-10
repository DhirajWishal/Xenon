// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Window.hpp"

#include <memory>

namespace Xenon
{
	namespace Platform
	{
		/**
		 * Supported platforms enum.
		 */
		enum class SupportedPlatforms : uint8_t
		{
			Windows,
			Linux,	// Not there yet.
			MacOS	// Not there yet.
		};

		/**
		 * Get the current platform.
		 *
		 * @return The platform.
		 */
		[[nodiscard]] SupportedPlatforms GetCurrentPlatform();

		/**
		 * Interface factory class.
		 * This class is used to create platform abstracted objects.
		 */
		class IFactory
		{
		public:
			/**
			 * Default constructor.
			 */
			IFactory() = default;

			/**
			 * Default virtual destructor.
			 */
			virtual ~IFactory() = default;

			/**
			 * Get the platform specific factory pointer.
			 *
			 * @return The factory pointer.
			 */
			[[nodiscard]] static IFactory* Get();

			/**
			 * Create a new window.
			 *
			 * @param title The title of the window.
			 * @param width The width of the window.
			 * @param height The height of the window.
			 * @return The window pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Window> createWindow(const std::string& title, uint32_t width, uint32_t height) = 0;
		};
	}
}