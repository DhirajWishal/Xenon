// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Layer.hpp"

#include "../XenonBackend/Camera.hpp"

#include "../XenonBackend/Swapchain.hpp"
#include "../XenonBackend/CommandRecorder.hpp"

#include <algorithm>

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
			auto pLayer = std::make_unique<LayerType>(std::forward<Arguments>(arguments)...);
			auto pRawPointer = pLayer.get();
			m_pLayers.emplace_back(std::move(pLayer));

			return pRawPointer;
		}

		/**
		 * Wait till the frame update has finished.
		 * This might be needed since the frame update is done asynchronously.
		 */
		void wait();

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

	private:
		/**
		 * Worker function.
		 * This function is called by the worker thread to update the attached systems.
		 */
		void worker();

	private:
		std::jthread m_Worker;
		std::condition_variable m_WorkerSynchronization;
		std::condition_variable m_ParentSynchronization;
		std::atomic_bool m_bShouldRun = true;
		std::mutex m_SynchronizationMutex;

		std::vector<std::unique_ptr<Layer>> m_pLayers;

		std::unique_ptr<Backend::Swapchain> m_pSwapChain = nullptr;
		std::unique_ptr<Backend::CommandRecorder> m_pCommandRecorder = nullptr;

		Backend::Camera* m_pCamera = nullptr;
	};
}