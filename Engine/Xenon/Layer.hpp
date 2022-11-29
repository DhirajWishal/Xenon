// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

#include "../XenonBackend/Image.hpp"
#include "../XenonBackend/CommandRecorder.hpp"

namespace Xenon
{
	class Renderer;

	/**
	 * Layer class.
	 * A renderer is made up of multiple layers (processing nodes). Each layer has a designated task, like to render a scene, a UI or for any other post processing step (like FXAA or shadow maps).
	 */
	class Layer : public XObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param renderer The renderer reference.
		 */
		explicit Layer(Renderer& renderer) : m_Renderer(renderer) {}

		/**
		 * Bind the layer to the command recorder.
		 * This is where all the required commands must be submitted to the command recorder to be executed by the GPU.
		 *
		 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
		 * @param pCommandRecorder The command recorder pointer to bind the commands to.
		 */
		virtual void bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder) = 0;

		/**
		 * Get the color attachment from the layer.
		 *
		 * @return The image pointer.
		 */
		[[nodiscard]] virtual Backend::Image* getColorAttachment() = 0;

	protected:
		Renderer& m_Renderer;
	};
}