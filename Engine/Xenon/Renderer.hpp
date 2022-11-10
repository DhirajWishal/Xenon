// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

#include "../XenonBackend/Rasterizer.hpp"
#include "../XenonBackend/Swapchain.hpp"

namespace Xenon
{
	/**
	 * Renderer class.
	 * This class performs rasterization on a scene using its camera.
	 */
	class Renderer final
	{
	public:
		/**
		 * Explicit constructor.
		 * This will automatically initialize the object and setup the window.
		 *
		 * @param instance The instance to create the rasterizing renderer with.
		 * @param title The title of the renderer window.
		 * @param width The width of the renderer window.
		 * @param height The height of the renderer window.
		 */
		explicit Renderer(Instance& instance, const std::string& title, uint32_t width, uint32_t height);

		/**
		 * Update the renderer.
		 *
		 * @return True if the render window is not closed.
		 * @return False if the render widow is closed.
		 */
		[[nodiscard]] bool update();

	private:
		std::unique_ptr<Backend::Swapchain> m_pSwapChain = nullptr;
		std::unique_ptr<Backend::Rasterizer> m_pRasterizer = nullptr;
	};
}