// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"
#include "../MeshStorage.hpp"

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
			Backend::RasterizingPipeline* m_pPipeline = nullptr;
			std::unique_ptr<Backend::CommandRecorder> m_pCommandRecorder = nullptr;

			std::unique_ptr<Backend::Descriptor> m_pCameraDescriptor = nullptr;
			std::vector<std::unique_ptr<Backend::Descriptor>> m_pMaterialDescriptors;
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
		 * Bind the layer to the command recorder.
		 * This is where all the required commands must be submitted to the command recorder to be executed by the GPU.
		 *
		 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
		 * @param pCommandRecorder The command recorder pointer to bind the commands to.
		 */
		void bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder) override;

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

	private:
		std::vector<DrawData> m_DrawData;
	};
}