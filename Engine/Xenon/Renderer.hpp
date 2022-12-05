// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Layer.hpp"

#include "../XenonCore/TaskGraph.hpp"

#include "../XenonBackend/Camera.hpp"
#include "../XenonBackend/Swapchain.hpp"
#include "../XenonBackend/CommandRecorder.hpp"

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

			return pRawPointer;
		}

		/**
		 * Clean everything to finish rendering.
		 */
		void cleanup();

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
		 * Get the renderer's task graph.
		 *
		 * @return The task graph.
		 */
		[[nodsicard]] TaskGraph& getTaskGraph() { return m_TaskGraph; }

		/**
		 * Get the renderer's task graph.
		 *
		 * @return The task graph.
		 */
		[[nodsicard]] const TaskGraph& getTaskGraph() const { return m_TaskGraph; }

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
		TaskGraph m_TaskGraph = TaskGraph(std::thread::hardware_concurrency());

		std::vector<std::unique_ptr<Layer>> m_pLayers;

		std::unique_ptr<Backend::Swapchain> m_pSwapChain = nullptr;
		std::unique_ptr<Backend::CommandRecorder> m_pCommandRecorder = nullptr;

		Backend::Camera* m_pCamera = nullptr;

		Instance& m_Instance;
	};
}