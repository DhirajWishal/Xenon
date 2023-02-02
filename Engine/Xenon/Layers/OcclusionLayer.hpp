// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"
#include "../Geometry.hpp"

#include "../../XenonBackend/RasterizingPipeline.hpp"
#include "../../XenonBackend/OcclusionQuery.hpp"

namespace Xenon
{
	/**
	 * Occlusion layer class.
	 * This class performs occlusion querying and assigns a unique ID for each geometry in the attached scene.
	 * Other layers can use the unique ID and this layer to check if the sub-mesh is culled or not.
	 */
	class OcclusionLayer final : public RasterizingLayer
	{
		/**
		 * Occlusion query samples structure.
		 * This contains information about the occlusion query, the number of samples and other information that were gathered from the occlusion query.
		 * This structure is intended to be explicit for one command buffer in a command recorder.
		 */
		struct OcclusionQuerySamples final
		{
			std::unordered_map<SubMesh, uint64_t> m_SubMeshSamples;
			std::unordered_map<SubMesh, uint32_t> m_SubMeshIndexMap;

			std::unique_ptr<Backend::OcclusionQuery> m_pOcclusionQuery = nullptr;

			std::vector<uint64_t> m_Samples;

			bool m_bHasQueryData = false;
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
		explicit OcclusionLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority = 5);

		/**
		 * On pre-update function.
		 * This object is called by the renderer before issuing it to the job system to be executed.
		 */
		void onPreUpdate() override;

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
		 * Get the samples of a single sub-mesh.
		 *
		 * @paarm subMesh The sub-mesh.
		 * @return The sample count.
		 */
		[[nodiscard]] uint64_t getSamples(const SubMesh& subMesh);

	private:
		/**
		 * Issue all the draw calls.
		 */
		void issueDrawCalls();

		/**
		 * Create a new per-geometry descriptor.
		 *
		 * @param group The group.
		 * @return The created descriptor.
		 */
		[[nodiscard]] std::unique_ptr<Backend::Descriptor> createPerGeometryDescriptor(Group group);

	private:
		std::mutex m_Mutex;

		std::unique_ptr<Backend::RasterizingPipeline> m_pOcclusionPipeline = nullptr;

		std::unordered_map<const Scene*, std::unique_ptr<Backend::Descriptor>> m_pOcclusionSceneDescriptors;

		std::unordered_map<Group, std::unique_ptr<Backend::Descriptor>> m_pPerGeometryDescriptors;

		std::vector<OcclusionQuerySamples> m_OcclusionQuerySamples;
	};
}