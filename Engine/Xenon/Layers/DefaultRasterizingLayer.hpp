// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"
#include "../MeshStorage.hpp"

#include "../../XenonCore/TaskNode.hpp"
#include "../../XenonCore/CountingFence.hpp"
#include "../../XenonBackend/RasterizingPipeline.hpp"

namespace Xenon
{
	/**
	 * Default rasterizing layer class.
	 * This object can store objects and render them using rasterizing.
	 */
	class DefaultRasterizingLayer final : public RasterizingLayer
	{
		/**
		 * Draw data structure.
		 */
		struct DrawData final
		{
			MeshStorage m_Storage;

			std::unique_ptr<Backend::Descriptor> m_pCameraDescriptor = nullptr;
		};

		/**
		 * Draw entry structure.
		 * This is used by the worker thread to draw.
		 */
		struct DrawEntry final
		{
			SubMesh m_SubMesh;

			Backend::VertexSpecification m_VertexSpecification;

			Backend::Buffer* m_pVertexBuffer = nullptr;
			Backend::Buffer* m_pIndexBuffer = nullptr;

			Backend::RasterizingPipeline* m_pPipeline = nullptr;

			Backend::Descriptor* m_pUserDefinedDescriptor = nullptr;
			std::unique_ptr<Backend::Descriptor> m_pMaterialDescriptor = nullptr;
			Backend::Descriptor* m_pCameraDescriptor = nullptr;

			uint64_t m_QueryIndex = 0;
		};

		/**
		 * Get the usable thread count.
		 *
		 * @return The usable thread count.
		 */
		[[nodiscard]] static uint32_t GetUsableThreadCount() { return std::thread::hardware_concurrency() / 2; }

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param renderer The renderer reference.
		 * @param pCamera The camera pointer used by the renderer.
		 */
		explicit DefaultRasterizingLayer(Renderer& renderer, Backend::Camera* pCamera);

		/**
		 * Destructor.
		 */
		~DefaultRasterizingLayer() override;

		/**
		 * Bind the layer to the command recorder.
		 * This is where all the required commands must be submitted to the command recorder to be executed by the GPU.
		 *
		 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
		 * @param pCommandRecorder The command recorder pointer to bind the commands to.
		 */
		void bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder) override;

		/**
		 * Update the layer.
		 * This is called by the renderer and all the required commands must be updated (if required) in this call.
		 *
		 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
		 * @param imageIndex The image's index.
		 * @param frameIndex The frame's index.
		 */
		void onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex) override;

		/**
		 * Add draw data to the layer to be rendered.
		 *
		 * @param storage The storage to render.
		 * @apram pPipeline The pipeline pointer to render with.
		 */
		void addDrawData(MeshStorage&& storage, Backend::RasterizingPipeline* pPipeline);

	private:
		/**
		 * Sub mesh binder function.
		 * This is a worker function which will run on another thread and will be used to bind sub-meshes to a secondary command recorder which then will be executed
		 * via the main graphics command recorder.
		 *
		 * @param index The index of the current thread.
		 */
		void subMeshBinder(uint8_t index);

		/**
		 * Setup the occlusion pipeline.
		 */
		void setupOcclusionPipeline();

		/**
		 * Draw the occlusion pass of the sub-mesh.
		 *
		 * @param pCommandRecorder The command recorder to bind to.
		 * @param lock The thread's resource lock.
		 * @param entry The draw entry.
		 */
		void occlusionPass(Backend::CommandRecorder* pCommandRecorder, std::unique_lock<std::mutex>& lock, const DrawEntry& entry);

		/**
		 * Draw the geometry pass of the sub-mesh.
		 *
		 * @param pCommandRecorder The command recorder to bind to.
		 * @param lock The thread's resource lock.
		 * @param entry The draw entry.
		 */
		void geometryPass(Backend::CommandRecorder* pCommandRecorder, std::unique_lock<std::mutex>& lock, const DrawEntry& entry) const;

	private:
		std::vector<std::jthread> m_Workers;
		std::mutex m_Mutex;
		std::condition_variable m_ConditionVariable;
		std::atomic_bool m_bShouldRun = true;
		CountingFence m_Synchronization;

		std::vector<DrawData> m_DrawData;
		std::vector<std::vector<DrawEntry>> m_DrawEntries = std::vector<std::vector<DrawEntry>>(GetUsableThreadCount());

		std::unique_ptr<Backend::RasterizingPipeline> m_pOcclusionPipeline = nullptr;
		std::unique_ptr<Backend::OcclusionQuery> m_pOcclusionQuery = nullptr;

		uint64_t m_SubMeshCount = 0;
	};
}