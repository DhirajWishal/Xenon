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
		 * @param priority The priority of the layer.
		 */
		explicit Layer(Renderer& renderer, uint32_t priority);

		/**
		 * On pre-update function.
		 * This object is called by the renderer before issuing it to the job system to be executed.
		 */
		virtual void onPreUpdate() {}

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
		 * Set the scene to perform operations on.
		 *
		 * @param scene The scene to attach.
		 */
		virtual void setScene(Scene& scene) { m_pScene = &scene; }

		/**
		 * Get the scene pointer.
		 *
		 * @return The scene pointer.
		 */
		[[nodiscard]] Scene* getScene() noexcept { return m_pScene; }

		/**
		 * Get the scene pointer.
		 *
		 * @return The scene pointer.
		 */
		[[nodiscard]] const Scene* getScene() const noexcept { return m_pScene; }

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

	private:
		uint32_t m_Priority = 0;

		bool m_IsActive = true;
	};
}