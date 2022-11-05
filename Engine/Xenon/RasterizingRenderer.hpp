// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

#include "../XenonBackend/Rasterizer.hpp"

namespace Xenon
{
	/**
	 * Rasterizing renderer class.
	 * This class performs rasterization on a scene using its camera.
	 */
	class RasterizingRenderer final
	{
	public:
		/**
		 * Explicit constructor.
		 * This will automatically initialize the object and setup the window.
		 *
		 * @param instance The instance to create the rasterizing renderer with.
		 */
		explicit RasterizingRenderer(Instance& instance);

	private:
		std::unique_ptr<Backend::Rasterizer> m_pRasterizer = nullptr;
		// m_ShadowSystem;
		// m_Camera;
		// m_PostProcessing;
		// m_RenderTarget;
		// m_Window;
		// 
	};
}