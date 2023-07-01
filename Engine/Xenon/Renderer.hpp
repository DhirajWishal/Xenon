// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Layer.hpp"

#include "../XenonCore/CountingFence.hpp"

#include "../XenonBackend/Camera.hpp"
#include "../XenonBackend/CommandSubmitter.hpp"

namespace Xenon
{
	/**
	 * Renderer class.
	 * This class renders a scene using its camera.
	 */
	class Renderer : public XObject
	{
	public:
		/**
		 * Explicit constructor.
		 * This will automatically initialize the object and setup the window.
		 *
		 * @param instance The instance to create the rasterizing renderer with.
		 * @param pCamera The camera pointer.
		 * @param width The width of the window.
		 * @param height The height of the window.
		 * @param title The title of the renderer window.
		 */
		explicit Renderer(Instance& instance, uint32_t width, uint32_t height, const std::string& title);

		/**
		 * Update the renderer.
		 *
		 * @return True if the render window is not closed.
		 * @return False if the render widow is closed.
		 */
		XENON_NODISCARD bool update();

		/**
		 * Create a new layer and attach it to the renderer.
		 * These layers are ordered from the highest (lowest priority value) to the lowest (highest priority value).
		 *
		 * @tparam LayerType The layer type to create.
		 * @tparam Arguments The layer's constructor argument types.
		 * @param arguments The constructor arguments to forward to the constructor when creating the layer.
		 * @return The created layer's pointer.
		 */
		template<class LayerType, class... Arguments>
		LayerType* createLayer(Arguments&&... arguments)
		{
			auto pLayer = std::make_unique<LayerType>(*this, std::forward<Arguments>(arguments)...);
			auto pRawPointer = pLayer.get();

			// Insert the layer.
			insertLayer(std::move(pLayer));

			return pRawPointer;
		}

		/**
		 * Clean everything to finish rendering.
		 */
		void cleanup();

		/**
		 * Close the renderer.
		 * This will only set an internal bool to false.
		 */
		void close();

	public:
		/**
		 * Get the instance to which the renderer is bound to.
		 *
		 * @return The instance reference.
		 */
		XENON_NODISCARD Instance& getInstance() { return m_Instance; }

		/**
		 * Get the instance to which the renderer is bound to.
		 *
		 * @return The instance reference.
		 */
		XENON_NODISCARD const Instance& getInstance() const { return m_Instance; }

		/**
		 * Get the window pointer.
		 *
		 * @return The window pointer.
		 */
		XENON_NODISCARD const Platform::Window* getWindow() const { return m_pSwapChain->getWindow(); }

		/**
		 * Get the mouse from the associated window.
		 *
		 * @return The mouse inputs.
		 */
		XENON_NODISCARD const Mouse& getMouse() const { return m_pSwapChain->getWindow()->getMouse(); }

		/**
		 * Get the keyboard from the associated window.
		 *
		 * @return The keyboard inputs.
		 */
		XENON_NODISCARD const Keyboard& getKeyboard() const { return m_pSwapChain->getWindow()->getKeyboard(); }

		/**
		 * Get the general events from the associated window.
		 *
		 * @return The general events.
		 */
		XENON_NODISCARD const GeneralEvents& getGeneralEvents() const { return m_pSwapChain->getWindow()->getGeneralEvents(); }

		/**
		 * Get the command recorder pointer.
		 *
		 * @return The pointer.
		 */
		XENON_NODISCARD Backend::CommandRecorder* getCommandRecorder() { return m_pCommandRecorder.get(); }

		/**
		 * Get the command recorder pointer.
		 *
		 * @return The pointer.
		 */
		XENON_NODISCARD const Backend::CommandRecorder* getCommandRecorder() const { return m_pCommandRecorder.get(); }

	private:
		/**
		 * Insert a layer to the internal vector.
		 *
		 * @param player the layer to insert.
		 */
		void insertLayer(std::unique_ptr<Layer>&& pLayer);

		/**
		 * Update the submit command recorders.
		 */
		void updateSubmitCommandRecorders();

		/**
		 * Update a layer on a separate job.
		 *
		 * @param pLayer The layer pointer to update.
		 * @param pPreviousLayer The previous layer pointer.
		 * @param imageIndex The image index.
		 * @param frameIndex The frame index.
		 */
		void updateLayer(Layer* pLayer, Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex);

		/**
		 * Copy the previous layer to the swapchain.
		 *
		 * @param pPreviousLayer The previous layer pointer.
		 */
		void copyToSwapchainAndSubmit(Layer* pPreviousLayer);

	private:
		CountingFence m_CountingFence;

		std::vector<std::unique_ptr<Layer>> m_pLayers;
		std::vector<std::unique_ptr<Backend::CommandSubmitter>> m_pCommandSubmitters;
		std::vector<std::vector<Backend::CommandRecorder*>> m_pSubmitCommandRecorders;

		std::unique_ptr<Backend::Swapchain> m_pSwapChain = nullptr;
		std::unique_ptr<Backend::CommandRecorder> m_pCommandRecorder = nullptr;

		Backend::Camera* m_pCamera = nullptr;

		Instance& m_Instance;

		bool m_IsOpen = true;
	};
}