// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonPlatform/Window.hpp"

#include <functional>

namespace Xenon
{
	namespace Platform
	{
		/**
		 * Linux window class.
		 */
		class LinuxWindow final : public Window
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param title The title of the window.
			 * @param width The window's width.
			 * @param height The window's height.
			 */
			explicit LinuxWindow(const std::string& title, uint32_t width, uint32_t height);

			/**
			 * Default virtual destructor.
			 */
			~LinuxWindow() override;

			/**
			 * Update the window.
			 * This will also poll the incoming window updates.
			 */
			void update() override;

			/**
			 * Check if the window is still open.
			 *
			 * @return True if the window is open.
			 * @return False if the window is closed.
			 */
			[[nodiscard]] bool isOpen() const override;

		private:
			bool m_IsOpen = true;
		};
	}
}