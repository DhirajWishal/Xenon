// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

namespace Xenon
{
	class Layer;

	/**
	 * Layer pass class.
	 * This class is used to bind data to command buffers regarding some type of a pass. Mainly this could be the geometry/ material pass, occlusion pass,
	 * shadow map pass or could be any other custom type.
	 */
	class LayerPass : public XObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param layer The layer reference to which the pass is attached to.
		 */
		explicit LayerPass(Layer& layer) : m_Layer(layer) {}

		/**
		 * On update pure virtual function.
		 * This class is invoked by the layer when it's onUpdate method is called.
		 *
		 * @param pPreviousLayer The previous layer pointer.
		 * @param imageIndex The swapchain image index.
		 * @param frameIndex The current frame index.
		 * @param pCommandRecorder The command recorder pointer to bind the commands to.
		 */
		virtual void onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex, Backend::CommandRecorder* pCommandRecorder) = 0;

		/**
		 * Get the parent layer reference.
		 *
		 * @return The layer reference.
		 */
		[[nodiscard]] Layer& getLayer() noexcept { return m_Layer; }

		/**
		 * Get the parent layer reference.
		 *
		 * @return The layer reference.
		 */
		[[nodiscard]] const Layer& getLayer() const noexcept { return m_Layer; }

	private:
		Layer& m_Layer;
	};
}