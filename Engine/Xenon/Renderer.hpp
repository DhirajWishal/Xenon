// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Layer.hpp"

#include "../XenonCore/TaskGraph.hpp"

#include "../XenonBackend/Camera.hpp"
#include "../XenonBackend/Swapchain.hpp"
#include "../XenonBackend/CommandRecorder.hpp"

#include <algorithm>
#include <latch>

namespace Xenon
{
	/**
	 * Renderer class.
	 * This class renders a scene using its camera.
	 */
	class Renderer final
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
		 * Destructor.
		 */
		~Renderer();

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
			auto lock = std::scoped_lock(m_SynchronizationMutex);
			auto pLayer = std::make_unique<LayerType>(*this, std::forward<Arguments>(arguments)...);
			auto pRawPointer = pLayer.get();
			m_pLayers.emplace_back(std::move(pLayer));

			return pRawPointer;
		}

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
		 * Get the attached camera descriptor pointer.
		 *
		 * @return The descriptor pointer.
		 */
		[[nodiscard]] Backend::Descriptor* getCameraDescriptor() { return m_pCameraDescriptor.get(); }

		/**
		 * Get the attached camera descriptor pointer.
		 *
		 * @return The descriptor pointer.
		 */
		[[nodiscard]] const Backend::Descriptor* getCameraDescriptor() const { return m_pCameraDescriptor.get(); }

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

	private:
		/**
		 * Worker function.
		 * This function is called by the worker thread to update the attached systems.
		 */
		void worker();

	private:
		std::jthread m_Worker;
		std::latch m_Latch = std::latch(1);
		std::atomic_bool m_bShouldRun = true;
		std::condition_variable m_WorkerSynchronization;
		std::mutex m_SynchronizationMutex;

		TaskGraph m_TaskGraph = TaskGraph(std::thread::hardware_concurrency());

		std::vector<std::unique_ptr<Layer>> m_pLayers;

		std::unique_ptr<Backend::Swapchain> m_pSwapChain = nullptr;
		std::unique_ptr<Backend::CommandRecorder> m_pCommandRecorder = nullptr;
		std::unique_ptr<Backend::Descriptor> m_pCameraDescriptor = nullptr;

		Backend::Camera* m_pCamera = nullptr;

		Instance& m_Instance;
	};
}