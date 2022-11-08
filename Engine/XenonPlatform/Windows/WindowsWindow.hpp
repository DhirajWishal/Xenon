// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Window.hpp"

namespace Xenon
{
	namespace Platform
	{
		/**
		 * Windows window class.
		 */
		class WindowsWindow final : public Window
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param title The title of the window.
			 * @param width The window's width.
			 * @param height The window's height.
			 */
			explicit WindowsWindow(const std::string& title, uint32_t width, uint32_t height);

			/**
			 * Default virtual destructor.
			 */
			~WindowsWindow() override = default;
		};
	}
}