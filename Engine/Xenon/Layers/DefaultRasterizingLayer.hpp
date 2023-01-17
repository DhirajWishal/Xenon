// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"
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
			Backend::Descriptor* m_pMaterialDescriptor = nullptr;
			Backend::Descriptor* m_pPerGeometryDescriptor = nullptr;
			Backend::Descriptor* m_pSceneDescriptor = nullptr;

			uint64_t m_QueryIndex = 0;
		};

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
		 * @param pCamera The camera pointer used by the renderer.
		 */
		explicit DefaultRasterizingLayer(Renderer& renderer, Backend::Camera* pCamera);

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

	private:
		/**
		 * Setup the occlusion pipeline.
		 */
		void setupOcclusionPipeline();

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
		void setupMaterialDescriptor(Pipeline& pipeline, SubMesh& subMesh, const MaterialSpecification& specification);

		/**
		 * Issue draw calls using a geometry's group.
		 *
		 * @param group The group.
		 */
		void issueDrawCalls(Group group);

		/**
		 * Issue the draw calls.
		 */
		void issueDrawCalls();

		/**
		 * Draw the occlusion pass of the sub-mesh.
		 *
		 * @param pCommandRecorder The command recorder pointer.
		 * @param geometry The geometry reference.
		 */
		void occlusionPass(Backend::CommandRecorder* pCommandRecorder, Geometry& geometry) const;

		/**
		 * Draw the geometry pass of the sub-mesh.
		 *
		 * @param pCommandRecorder The command recorder pointer.
		 * @param pPerGeometryDescriptor The per-geometry descriptor pointer.
		 * @param geometry The geometry reference.
		 * @param pipeline The pipeline to draw with.
		 */
		void geometryPass(Backend::CommandRecorder* pCommandRecorder, Backend::Descriptor* pPerGeometryDescriptor, Geometry& geometry, Pipeline& pipeline);

	private:
		std::mutex m_Mutex;
		CountingFence m_Synchronization;

		std::unordered_map<std::thread::id, std::unique_ptr<Backend::CommandRecorder>> m_pThreadLocalCommandRecorder;

		std::unordered_map<Material, Pipeline> m_pPipelines;

		std::unique_ptr<Backend::RasterizingPipeline> m_pOcclusionPipeline = nullptr;
		std::unique_ptr<Backend::OcclusionQuery> m_pOcclusionQuery = nullptr;
		std::unique_ptr<Backend::Descriptor> m_pOcclusionCameraDescriptor = nullptr;

		std::atomic_uint64_t m_DrawCount = 0;
	};
}