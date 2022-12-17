// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonPlatform/Window.hpp"

#include <wayland-client.h>
#include <wayland-server.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>

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
			explicit LinuxWindow(const std::string &title, uint32_t width, uint32_t height);

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

			/**
			 * Get the Display object.
			 *
			 * @return The display object pointer.
			 */
			[[nodiscard]] wl_display *getDisplay() noexcept { return m_pDisplay; }

			/**
			 * Get the Display object.
			 *
			 * @return The display object pointer.
			 */
			[[nodiscard]] const wl_display *getDisplay() const noexcept { return m_pDisplay; }

			/**
			 * Set the Compositor object.
			 *
			 * @param pCompositor The compositor object pointer.
			 */
			void setCompositor(wl_compositor *pCompositor) noexcept { m_pCompositor = pCompositor; }

			/**
			 * Get the Compositor object.
			 *
			 * @return The compositor object pointer.
			 */
			[[nodiscard]] wl_compositor *getCompositor() noexcept { return m_pCompositor; }

			/**
			 * Get the Compositor object.
			 *
			 * @return The compositor object pointer.
			 */
			[[nodiscard]] const wl_compositor *getCompositor() const noexcept { return m_pCompositor; }

			/**
			 * Get the Surface object.
			 *
			 * @return The surface object pointer.
			 */
			[[nodiscard]] wl_surface *getSurface() noexcept { return m_pSurface; }

			/**
			 * Get the Surface object.
			 *
			 * @return The surface object pointer.
			 */
			[[nodiscard]] const wl_surface *getSurface() const noexcept { return m_pSurface; }

			/**
			 * Set the Shell object.
			 *
			 * @param pShell The shell pointer.
			 */
			void setShell(wl_shell *pShell) noexcept { m_pShell = pShell; }

			/**
			 * Get the Shell object.
			 *
			 * @return The shell object pointer.
			 */
			[[nodiscard]] wl_shell *getShell() noexcept { return m_pShell; }

			/**
			 * Get the Shell object.
			 *
			 * @return The shell object pointer.
			 */
			[[nodiscard]] const wl_shell *getShell() const noexcept { return m_pShell; }

		private:
			wl_display *m_pDisplay = nullptr;
			wl_compositor *m_pCompositor = nullptr;
			wl_surface *m_pSurface = nullptr;
			wl_shell *m_pShell = nullptr;
			wl_shell_surface *m_pShellSurface = nullptr;

			bool m_IsOpen = true;
		};
	}
}