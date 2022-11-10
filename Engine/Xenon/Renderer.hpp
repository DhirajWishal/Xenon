// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "../XenonBackend/Camera.hpp"

#include "../XenonBackend/Rasterizer.hpp"
#include "../XenonBackend/Swapchain.hpp"

namespace Xenon
{
	/**
	 * Renderer class.
	 * This class renders a scene using its camera.
	 */
	class Renderer final
	{
	public:
		/**
		 * Explicit constructor.
		 * This will automatically initialize the object and setup the window.
		 *
		 * @param instance The instance to create the rasterizing renderer with.
		 * @param pCamera The camera pointer.
		 * @param title The title of the renderer window.
		 */
		explicit Renderer(Instance& instance, Backend::Camera* pCamera, const std::string& title);

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