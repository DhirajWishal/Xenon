// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"

#include "../../XenonBackend/RasterizingPipeline.hpp"

namespace Xenon
{
	namespace Experimental
	{
		class DirectLightingLayer;

		/**
		 * Light look up table class.
		 * This class cache's light's occlusion data.
		 *
		 * The process goes as follows.
		 * 1. Compute the world position of a given vertex where the light is the origin.
		 * 2. Take the angle between the light position (origin) and the vertex position and calculate the angle.
		 * 3. Find the correct pixel to which the angle corresponds and mark it as occluded (non-zero value).
		 *
		 * This also refresh the image by setting a single value (0) before each run. And each class is intended to be used by one light.
		 */
		class LightLUT final : public RasterizingLayer
		{
			/**
			 * Control block structure.
			 */
			struct ControlBlock final
			{
				XENON_HLSL_VEC3_ALIGNMENT uint32_t m_Stride;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param renderer The renderer reference.
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 * @param priority The priority of the layer.
			 */
			explicit LightLUT(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority);

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
			 * Set the renderable scene to the layer.
			 *
			 * @param scene The scene reference.
			 */
			void setScene(Scene& scene) override;

			/**
			 * Set the attachment direct lighting layer.
			 *
			 * @param pLayer The layer pointer.
			 */
			void setAttachment(DirectLightingLayer* pLayer);

			/**
			 * Get the control block pointer.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] Backend::Buffer* getControlBlock() noexcept { return m_pControlBlock.get(); }

			/**
			 * Get the control block pointer.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] const Backend::Buffer* getControlBlock() const noexcept { return m_pControlBlock.get(); }

			/**
			 * Get the look up table pointer.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] Backend::Buffer* getLookUpTable() noexcept { return m_pLookUpTable.get(); }

			/**
			 * Get the look up table pointer.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] const Backend::Buffer* getLookUpTable() const noexcept { return m_pLookUpTable.get(); }

		private:
			/**
			 * Issue all the draw calls.
			 */
			void issueDrawCalls();

		private:
			ControlBlock m_ControlBlock;

			std::unique_ptr<Backend::Buffer> m_pLookUpTable;
			std::unique_ptr<Backend::Buffer> m_pControlBlock;

			std::unique_ptr<Backend::RasterizingPipeline> m_pPipeline = nullptr;

			std::unique_ptr<Backend::Descriptor> m_pSceneDescriptor = nullptr;
			std::unique_ptr<Backend::Descriptor> m_pUserDefinedDescriptor = nullptr;

			DirectLightingLayer* m_pAttachment = nullptr;
		};
	}
}