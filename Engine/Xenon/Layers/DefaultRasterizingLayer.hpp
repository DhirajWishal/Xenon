// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"

namespace Xenon
{
	/**
	 * Default rasterizing layer class.
	 * This object can store objects and render them using rasterizing.
	 */
	class DefaultRasterizingLayer final : public RasterizingLayer
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param pCamera The camera pointer used by the renderer.
		 */
		explicit DefaultRasterizingLayer(Instance& instance, Backend::Camera* pCamera);

		/**
		 * Bind the layer to the command recorder.
		 * This is where all the required commands must be submitted to the command recorder to be executed by the GPU.
		 *
		 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
		 * @param pCommandRecorder The command recorder pointer to bind the commands to.
		 */
		void bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder) override;

	private:
		glm::vec4 m_ClearColor;
	};
}