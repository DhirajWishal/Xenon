// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RayTracingLayer.hpp"
#include "../MeshStorage.hpp"
#include "../../XenonBackend/RayTracingPipeline.hpp"

namespace Xenon
{
	/**
	 * Default ray tracing layer.
	 * This layer can be used to render a scene using ray tracing.
	 */
	class DefaultRayTracingLayer final : public RayTracingLayer
	{
		/**
		 * Draw data structure.
		 */
		struct DrawData final
		{
			MeshStorage m_MeshStorage;

			std::unique_ptr<Backend::BottomLevelAccelerationStructure> m_pBottomLevelAccelerationStructure;
			std::unique_ptr<Backend::TopLevelAccelerationStructure> m_pTopLevelAccelerationStructure;
			std::unique_ptr<Backend::ShaderBindingTable> m_pShaderBindingTable;

			Backend::RayTracingPipeline* m_pPipeline = nullptr;
		};

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param renderer The renderer reference.
		 * @param pCamera The camera which is used to render the scene.
		 */
		explicit DefaultRayTracingLayer(Renderer& renderer, Backend::Camera* pCamera);

		/**
		 * Destructor.
		 */
		~DefaultRayTracingLayer() override = default;

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
		void addDrawData(MeshStorage&& storage, Backend::RayTracingPipeline* pPipeline);

		/**
		 * Get the total draw count.
		 * This is the number of sub-meshes the layer will render.
		 *
		 * @return The count.
		 */
		[[nodiscard]] uint64_t getTotalDrawCount() const noexcept { return m_SubMeshCount; }

		/**
		 * Get the draw count.
		 * This is the number of sub-meshes the layer rendered in the previous frame.
		 *
		 * @return The count.
		 */
		[[nodiscard]] uint64_t getDrawCount() const noexcept { return m_DrawCount; }

	private:
		std::mutex m_Mutex;
		std::vector<DrawData> m_DrawData;

		std::atomic_uint64_t m_DrawCount = 0;
		uint64_t m_SubMeshCount = 0;
	};
}