// Copyright 2022 Dhiraj Wishal
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
	class Renderer final : public XObject
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
		explicit Renderer(Instance& instance, Backend::Camera* pCamera, const std::string& title);

		/**
		 * Update the renderer.
		 *
		 * @return True if the render window is not closed.
		 * @return False if the render widow is closed.
		 */
		[[nodiscard]] bool update();

		/**
		 * Create a new layer and attach it to the renderer.
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

			m_pLayers.emplace_back(std::move(pLayer));

			// Reset the command recorders.
			m_pSubmitCommandRecorders.clear();
			m_pSubmitCommandRecorders.reserve(m_pLayers.size() + 1);

			for (const auto& pRegisteredLayer : m_pLayers)
				m_pSubmitCommandRecorders.emplace_back(pRegisteredLayer->getCommandRecorder());

			m_pSubmitCommandRecorders.emplace_back(m_pCommandRecorder.get());

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
		 * Get the attached camera pointer.
		 *
		 * @return The camera pointer.
		 */
		[[nodiscard]] Backend::Camera* getCamera() { return m_pCamera; }

		/**
		 * Get the attached camera pointer.
		 *
		 * @return The camera pointer.
		 */
		[[nodiscard]] const Backend::Camera* getCamera() const { return m_pCamera; }

		/**
		 * Get the instance to which the renderer is bound to.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] Instance& getInstance() { return m_Instance; }

		/**
		 * Get the instance to which the renderer is bound to.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] const Instance& getInstance() const { return m_Instance; }

		/**
		 * Get the window pointer.
		 *
		 * @return The window pointer.
		 */
		[[nodiscard]] const Platform::Window* getWindow() const { return m_pSwapChain->getWindow(); }

		/**
		 * Get the mouse from the associated window.
		 *
		 * @return The mouse inputs.
		 */
		[[nodiscard]] const Mouse& getMouse() const { return m_pSwapChain->getWindow()->getMouse(); }

		/**
		 * Get the keyboard from the associated window.
		 *
		 * @return The keyboard inputs.
		 */
		[[nodiscard]] const Keyboard& getKeyboard() const { return m_pSwapChain->getWindow()->getKeyboard(); }

		/**
		 * Get the command recorder pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] Backend::CommandRecorder* getCommandRecorder() { return m_pCommandRecorder.get(); }

		/**
		 * Get the command recorder pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] const Backend::CommandRecorder* getCommandRecorder() const { return m_pCommandRecorder.get(); }

	private:
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
		std::vector<Backend::CommandRecorder*> m_pSubmitCommandRecorders;

		std::unique_ptr<Backend::Swapchain> m_pSwapChain = nullptr;
		std::unique_ptr<Backend::CommandRecorder> m_pCommandRecorder = nullptr;

		Backend::Camera* m_pCamera = nullptr;

		Instance& m_Instance;

		bool m_IsOpen = true;
	};
}