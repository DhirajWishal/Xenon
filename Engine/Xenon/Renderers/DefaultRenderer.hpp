// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Renderer.hpp"

namespace Xenon
{
	namespace Experimental
	{
		/**
		 * Default renderer class.
		 * This class performs global illumination and an additional layer can be added for other processing.
		 */
		class DefaultRenderer final : public Renderer
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
			explicit DefaultRenderer(Instance& instance, Backend::Camera* pCamera, const std::string& title);
		};
	}
}