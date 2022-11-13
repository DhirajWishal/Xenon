// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

#include "../XenonBackend/CommandRecorder.hpp"

namespace Xenon
{
	/**
	 * Layer class.
	 * A renderer is made up of multiple layers (processing nodes). Each layer has a designated task, like to render a scene, a UI or for any other post processing step (like FXAA or shadow maps).
	 * The layers are sorted according to their priority (least to the most; 0 - 255).
	 */
	class Layer : public XObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param priority The priority of the layer.
		 */
		explicit Layer(Instance& instance, uint8_t priority) : m_Instance(instance), m_Priority(priority) {}

		/**
		 * Bind the layer to the command recorder.
		 * This is where all the required commands must be submitted to the command recorder to be executed by the GPU.
		 *
		 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
		 * @param pCommandRecorder The command recorder pointer to bind the commands to.
		 */
		virtual void bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder) = 0;

		/**
		 * Get the priority of the layer.
		 *
		 * @return The priority.
		 */
		[[nodiscard]] uint8_t getPriority() const { return m_Priority; }

	protected:
		Instance& m_Instance;
		const uint8_t m_Priority;
	};
}