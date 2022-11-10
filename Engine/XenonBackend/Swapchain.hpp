// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

#include "../XenonPlatform/Window.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Swapchain class.
		 * Swapchains are used to render frames to a window.
		 *
		 * Note that a swapchain owns a window and it can be accessed via the appropriate functions.
		 */
		class Swapchain : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param title The title of the window.
			 * @param width The window's width.
			 * @param height The window's height.
			 */
			explicit Swapchain([[maybe_unused]] Device* pDevice, const std::string& title, uint32_t width, uint32_t height);

			/**
			 * Default virtual destructor.
			 */
			virtual ~Swapchain() override = default;

			/**
			 * Get the swapchain's window pointer.
			 *
			 * @return The window pointer.
			 */
			[[nodiscard]] Platform::Window* getWindow() { return m_pWindow.get(); }

			/**
			 * Get the swapchain's window pointer.
			 *
			 * @return The const window pointer.
			 */
			[[nodiscard]] const Platform::Window* getWindow() const { return m_pWindow.get(); }

		protected:
			std::unique_ptr<Platform::Window> m_pWindow = nullptr;
		};
	}
}