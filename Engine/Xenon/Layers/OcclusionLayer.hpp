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
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param renderer The renderer reference.
		 * @param pCamera The camera pointer used by the renderer.
		 * @param priority The priority of the layer. Default is 5.
		 */
		explicit OcclusionLayer(Renderer& renderer, Backend::Camera* pCamera, uint32_t priority = 5);

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
		 * Get the samples of a single index (sub-mesh).
		 *
		 * @paarm index The sub-mesh index.
		 * @return The sample count.
		 */
		[[nodiscard]] uint64_t getSamples(uint32_t index) const;

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
		std::unique_ptr<Backend::OcclusionQuery> m_pOcclusionQuery = nullptr;

		std::unordered_map<const Scene*, std::unique_ptr<Backend::Descriptor>> m_pOcclusionSceneDescriptors;

		std::unordered_map<Group, std::unique_ptr<Backend::Descriptor>> m_pPerGeometryDescriptors;
	};
}