// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "Scene.hpp"

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
		explicit Layer(Renderer& renderer);

		/**
		 * Update the layer.
		 * This is called by the renderer and all the required commands must be updated (if required) in this call.
		 *
		 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
		 * @param imageIndex The image's index.
		 * @param frameIndex The frame's index.
		 */
		virtual void onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex) = 0;

		/**
		 * Get the color attachment from the layer.
		 *
		 * @return The image pointer.
		 */
		[[nodiscard]] virtual Backend::Image* getColorAttachment() = 0;

		/**
		 * Get all the command buffers that will be batched and submitted.
		 * This method is called by the renderer and is used to collect all the command buffers that will be batched together when submitting.
		 *
		 * @param pCommandBuffers The command buffers vector to which the command buffers need to be attached.
		 */
		virtual void onRegisterCommandBuffers(std::vector<Backend::CommandRecorder*>& pCommandBuffers) { pCommandBuffers.emplace_back(m_pCommandRecorder.get()); }

		/**
		 * Set the scene to perform operations on.
		 *
		 * @param scene The scene to attach.
		 */
		void setScene(Scene& scene) { m_pScene = &scene; }

		/**
		 * Notify the renderer to render this layer.
		 */
		void activate() noexcept { m_IsActive = true; }

		/**
		 * Notify the renderer to not render this layer.
		 */
		void deactivate() noexcept { m_IsActive = false; }

		/**
		 * Check if the layer is active (should be rendered or not).
		 *
		 * @return True if the layer is active.
		 * @return False if the layer is inactive (should not be rendered).
		 */
		[[nodiscard]] bool isActive() const noexcept { return m_IsActive; }

		/**
		 * Get the renderer to which the layer is bound to.
		 *
		 * @return The layer.
		 */
		[[nodiscard]] Renderer& getRenderer() noexcept { return m_Renderer; }

		/**
		 * Get the renderer to which the layer is bound to.
		 *
		 * @return The layer.
		 */
		[[nodiscard]] const Renderer& getRenderer() const noexcept { return m_Renderer; }

		/**
		 * Get the command recorder pointer.
		 *
		 * @return The command recorder pointer.
		 */
		[[nodiscard]] Backend::CommandRecorder* getCommandRecorder() noexcept { return m_pCommandRecorder.get(); }

		/**
		 * Get the command recorder pointer.
		 *
		 * @return The command recorder pointer.
		 */
		[[nodiscard]] const Backend::CommandRecorder* getCommandRecorder() const noexcept { return m_pCommandRecorder.get(); }

		/**
		 * Get the priority of the current layer.
		 * If two layers have the same priority, it means that it does not depend on each other. The renderer will batch all the command recorders of the
		 * two layers and submit them in one call.
		 *
		 * @return The priority index.
		 */
		[[nodiscard]] uint32_t getPriority() const noexcept { return m_Priority; }

		/**
		 * Select the next command buffer.
		 * This is called by the renderer and the overriding class doesn't need to do this (and shouldn't!).
		 */
		void selectNextCommandBuffer() { m_pCommandRecorder->next(); }

	protected:
		Renderer& m_Renderer;
		Scene* m_pScene = nullptr;

		std::unique_ptr<Backend::CommandRecorder> m_pCommandRecorder = nullptr;

		std::vector<Backend::CommandRecorder*> m_pSubmitCommandrecorders;

		uint32_t m_Priority = 0;

	private:
		bool m_IsActive = true;

	};
}