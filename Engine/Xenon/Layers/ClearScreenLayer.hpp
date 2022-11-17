// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"

#include <glm/vec4.hpp>

namespace Xenon
{
	/**
	 * Clear screen layer class.
	 * This can be used to clear/ initialize the screen (the swapchain or any subsequent layer).
	 */
	class ClearScreenLayer final : public RasterizingLayer
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param pCamera The camera pointer used by the renderer.
		 * @param color The color to set.
		 */
		explicit ClearScreenLayer(Instance& instance, Backend::Camera* pCamera, const glm::vec4& color);

		/**
		 * Bind the layer to the command recorder.
		 * This is where all the required commands must be submitted to the command recorder to be executed by the GPU.
		 *
		 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
		 * @param pCommandRecorder The command recorder pointer to bind the commands to.
		 */
		void bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder) override;

	public:
		/**
		 * Get the clear color.
		 *
		 * @return The color.
		 */
		[[nodiscard]] glm::vec4 getClearColor() const { return m_ClearColor; }

		/**
		 * Set the clear color.
		 *
		 * @param color The color to set.
		 */
		void setClearColor(const glm::vec4& color) { m_ClearColor = color; }

	private:
		glm::vec4 m_ClearColor;
	};
}