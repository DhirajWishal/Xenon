// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"
#include "../Geometry.hpp"

#include "../../XenonBackend/RasterizingPipeline.hpp"

namespace Xenon
{
	namespace Experimental
	{
		/**
		 * Shadow map layer.
		 * This layer contains the logic to generate a shadow map using the light(s) attached to the scene.
		 * If not scene or light is attached, this will just be a blank depth map.
		 *
		 * Currently it only supports one light.
		 */
		class ShadowMapLayer final : public RasterizingLayer
		{
			/**
			 * Shadow camera structure.
			 */
			struct ShadowCamera final
			{
				glm::mat4 m_View;
				glm::mat4 m_Projection;
			};

			/**
			 * Camera information structure.
			 */
			struct CameraInformation final
			{
				ShadowCamera m_Camera;

				std::unique_ptr<Backend::Descriptor> m_pDescriptor = nullptr;
				std::unique_ptr<Backend::Buffer> m_pBuffer = nullptr;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param renderer The renderer reference.
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 * @param priority The priority of the layer. Default is 4.
			 */
			explicit ShadowMapLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority = 4);

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
			 * Get the shadow image from the rasterizer.
			 *
			 * @return The shadow image pointer.
			 */
			XENON_NODISCARD Backend::Image* getShadowImage() { return m_pRasterizer->getImageAttachment(Backend::AttachmentType::Depth); }

			/**
			 * Get the shadow camera's buffer.
			 *
			 * @return The buffer pointer.
			 */
			XENON_NODISCARD Backend::Buffer* getShadowCameraBuffer() noexcept { return m_LightCamera.m_pBuffer.get(); }

			/**
			 * Get the shadow texture from the layer.
			 *
			 * @return The texture.
			 */
			XENON_NODISCARD Texture getShadowTexture();

		private:
			/**
			 * Issue all the required draw calls.
			 */
			void issueDrawCalls();

			/**
			 * Calculate the shadow camera using the light source.
			 *
			 * @param lightSource The light source.
			 * @return The shadow camera.
			 */
			XENON_NODISCARD ShadowCamera calculateShadowCamera(const Components::LightSource& lightSource) const;

			/**
			 * Create a per-geometry descriptor.
			 *
			 * @param group The group.
			 * @return The descriptor pointer.
			 */
			XENON_NODISCARD std::unique_ptr<Backend::Descriptor> createPerGeometryDescriptor(Group group);

		private:
			CameraInformation m_LightCamera;

			std::unique_ptr<Backend::ImageView> m_pImageView = nullptr;
			std::unique_ptr<Backend::ImageSampler> m_pImageSampler = nullptr;

			std::unique_ptr<Backend::RasterizingPipeline> m_pPipeline = nullptr;
			std::unordered_map<Group, std::unique_ptr<Backend::Descriptor>> m_pPerGeometryDescriptors;

			Components::Transform m_DefaultTransform;
			std::unique_ptr<Backend::Buffer> m_pDefaultTransformBuffer;

			Group m_LightGroup;
		};
	}
}