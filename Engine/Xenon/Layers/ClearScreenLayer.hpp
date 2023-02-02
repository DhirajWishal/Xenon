// Copyright 2022-2023 Dhiraj Wishal
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
		 * @param renderer The renderer reference.
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 * @param color The color to set.
		 * @param priority The priority of the layer.
		 */
		explicit ClearScreenLayer(Renderer& renderer, uint32_t width, uint32_t height, const glm::vec4& color, uint32_t priority);

		/**
		 * Update the layer.
		 * This is called by the renderer and all the required commands must be updated (if required) in this call.
		 *
		 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
		 * @param imageIndex The image's index.
		 * @param frameIndex The frame's index.
		 */
		void onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex) override;

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