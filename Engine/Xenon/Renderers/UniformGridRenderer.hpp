// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Renderer.hpp"

namespace Xenon
{
	/**
	 * Uniform grid renderer class.
	 * This class uses a uniform grid as the acceleration structure to perform ray tracing.
	 */
	class UniformGridRenderer final : public Renderer
	{
	public:
		/**
		 * Explicit constructor.
		 * This will automatically initialize the object and setup the window.
		 *
		 * @param instance The instance to create the rasterizing renderer with.
		 * @param width The width of the window.
		 * @param height The height of the window.
		 * @param title The title of the renderer window.
		 */
		explicit UniformGridRenderer(Instance& instance, uint32_t width, uint32_t height, const std::string& title);
	};
}