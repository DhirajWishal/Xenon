// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonPlatform/Window.hpp"

struct SDL_Window;

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
			XENON_NODISCARD bool isOpen() const override;

			/**
			 * Get the SDL window handle.
			 *
			 * @return The window handle.
			 */
			XENON_NODISCARD SDL_Window* getWindowHandle() { return m_pWindow; }

			/**
			 * Get the SDL window handle.
			 *
			 * @return The window handle.
			 */
			XENON_NODISCARD const SDL_Window* getWindowHandle() const { return m_pWindow; }

		private:
			/**
			 * Handle a key event and decode it to the proper keyboard input.
			 *
			 * @param scanCode The key's scan code.
			 * @param pressed Whether the key is pressed or not.
			 */
			void handleKeyEvent(int32_t scanCode, bool pressed);

			/**
			 * Handle special character events.
			 *
			 * @param characters The special character keys.
			 * @param pressed Whether the key is pressed or not.
			 */
			void handleSpecialCharacter(uint64_t characters, bool pressed);

			/**
			 * Handle the mouse button event.
			 *
			 * @param button The button to which the event is received.
			 * @param clicks The number of clicks recorded.
			 */
			void handleMouseButtonEvent(uint8_t button, uint8_t clicks);

		private:
			SDL_Window* m_pWindow = nullptr;

			bool m_IsOpen = true;
		};
	}
}