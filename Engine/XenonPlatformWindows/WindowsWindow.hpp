// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonPlatform/Window.hpp"

#ifndef UNICODE
#define UNICODE
#endif 

#include <Windows.h>

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
			~WindowsWindow() override;

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

			/**
			 * Get the window handle.
			 *
			 * @return The window handle.
			 */
			[[nodiscard]] HWND getWindowHandle() const { return m_WindowHandle; }

		public:
			/**
			 * Handle the event sent to the WindowProc.
			 *
			 * @param uMsg The message.
			 * @param wParam The wParam.
			 * @param lParam The lParam.
			 * @return The result.
			 */
			LRESULT handleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

		private:
			HWND m_WindowHandle = nullptr;

			bool m_IsOpen = true;
		};
	}
}