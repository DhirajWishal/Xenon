// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OcclusionLayer.hpp"

#include "../Geometry.hpp"

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
		 * Pipeline structure.
		 * This contains information regarding a single pipeline and it's descriptors.
		 */
		struct Pipeline final
		{
			std::unique_ptr<Backend::RasterizingPipeline> m_pPipeline = nullptr;
			std::unique_ptr<Backend::Descriptor> m_pSceneDescriptor = nullptr;
			std::unordered_map<Group, std::unique_ptr<Backend::Descriptor>> m_pPerGeometryDescriptors;
			std::unordered_map<SubMesh, std::unique_ptr<Backend::Descriptor>> m_pMaterialDescriptors;
		};

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param renderer The renderer reference.
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 * @param priority The priority of the layer. Default is 5.
		 */
		explicit DefaultRasterizingLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority = 5);

		/**
		 * Destructor.
		 */
		~DefaultRasterizingLayer() override = default;

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
		 * Get the draw count.
		 * This is the number of sub-meshes the layer rendered in the previous frame.
		 *
		 * @return The count.
		 */
		[[nodiscard]] uint64_t getDrawCount() const noexcept { return m_DrawCount; }

		/**
		 * Set the occlusion layer to get occlusion results from.
		 * This is needed if you need to enable occlusion.
		 *
		 * @param pOcclusionLayer The layer to set.
		 */
		void setOcclusionLayer(OcclusionLayer* pOcclusionLayer) noexcept { m_pOcclusionLayer = pOcclusionLayer; }

	private:
		/**
		 * Create the per-geometry descriptor.
		 *
		 * @param pipeline The pipeline reference.
		 * @param group The geometry's group.
		 */
		[[nodiscard]] std::unique_ptr<Backend::Descriptor> createPerGeometryDescriptor(Pipeline& pipeline, Group group);

		/**
		 * Setup the material descriptor.
		 *
		 * @param pipeline The pipeline.
		 * @param subMesh The sub-mesh.
		 * @param specification The material specification.
		 */
		void setupMaterialDescriptor(Pipeline& pipeline, SubMesh& subMesh, const MaterialSpecification& specification) const;

		/**
		 * Issue the draw calls.
		 */
		void issueDrawCalls();

		/**
		 * Draw the geometry pass of the sub-mesh.
		 *
		 * @param pPerGeometryDescriptor The per-geometry descriptor pointer.
		 * @param geometry The geometry reference.
		 * @param pipeline The pipeline to draw with.
		 */
		void geometryPass(Backend::Descriptor* pPerGeometryDescriptor, Geometry& geometry, Pipeline& pipeline);

	private:
		std::mutex m_Mutex;
		CountingFence m_Synchronization;

		std::unordered_map<std::thread::id, std::unique_ptr<Backend::CommandRecorder>> m_pThreadLocalCommandRecorder;

		std::unordered_map<Material, Pipeline> m_pPipelines;

		std::atomic_uint64_t m_DrawCount = 0;

		OcclusionLayer* m_pOcclusionLayer = nullptr;
	};
}