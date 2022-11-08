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
		 */
		class Swapchain : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pWindow The window pointer.
			 */
			explicit Swapchain([[maybe_unused]] Device* pDevice, Platform::Window* pWindow) : m_pWindow(pWindow) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Swapchain() override = default;

		protected:
			Platform::Window* m_pWindow = nullptr;
		};
	}
}