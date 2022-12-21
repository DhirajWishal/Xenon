// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"
#include "../MeshStorage.hpp"

#include "../../XenonCore/TaskNode.hpp"
#include "../../XenonBackend/RasterizingPipeline.hpp"

#include <latch>

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
			Backend::RasterizingPipeline* m_pPipeline = nullptr;
			std::unique_ptr<Backend::CommandRecorder> m_pCommandRecorder = nullptr;

			std::unique_ptr<Backend::Descriptor> m_pCameraDescriptor = nullptr;
			std::vector<std::unique_ptr<Backend::Descriptor>> m_pMaterialDescriptors;
			std::vector<std::shared_ptr<TaskNode>> m_pNodes;
		};

		/**
		 * Draw entry structure.
		 * This is used by the worker thread to draw.
		 */
		struct DrawEntry final
		{
			SubMesh m_SubMesh;
			DrawData* m_pDrawData = nullptr;

			Backend::Descriptor* m_pUserDefinedDescriptor = nullptr;
			Backend::Descriptor* m_pMaterialDescriptor = nullptr;
			Backend::Descriptor* m_pCameraDescriptor = nullptr;
		};

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
		 * Bind the draw data on another thread.
		 *
		 * @param drawData The draw data reference.
		 * @param pCommandRecorder The command recorder pointer.
		 */
		void bindDrawData(DrawData& drawData, Backend::CommandRecorder* pCommandRecorder) const;

		/**
		 * Sub mesh binder function.
		 * This is a worker function which will run on another thread and will be used to bind sub-meshes to a secondary command recorder which then will be executed
		 * via the main graphics command recorder.
		 */
		void subMeshBinder();

	private:
		std::vector<std::jthread> m_Workers;
		std::mutex m_Mutex;
		std::condition_variable m_ConditionVariable;
		std::unique_ptr<std::latch> m_pLatch = nullptr;
		std::atomic_bool m_bShouldRun = true;

		std::vector<DrawData> m_DrawData;
		std::list<DrawEntry> m_DrawEntries;
	};
}