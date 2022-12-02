// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandRecorder.hpp"
#include "Buffer.hpp"
#include "Rasterizer.hpp"
#include "Swapchain.hpp"
#include "Descriptor.hpp"
#include "ImageView.hpp"
#include "ImageSampler.hpp"
#include "RasterizingPipeline.hpp"
#include "ComputePipeline.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Interface factory class.
		 * This factory creates backend classes from a single backend.
		 */
		class IFactory
		{
		public:
			/**
			 * Default constructor.
			 */
			IFactory() = default;

			/**
			 * Default virtual constructor.
			 */
			virtual ~IFactory() = default;

			/**
			 * Create a new instance.
			 *
			 * @param appliationName The name of the application.
			 * @param applicationVersion The application version.
			 * @return The instance pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Instance> createInstance(const std::string& applicationName, uint32_t applicationVersion) = 0;

			/**
			 * Create a new device.
			 *
			 * @param pInstance The instance pointer to which the device is bound to.
			 * @param requriedRenderTargets The render targets which are required to have.
			 * @return The device pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Device> createDevice(Instance* pInstance, RenderTargetType requiredRenderTargets) = 0;

			/**
			 * Create a new command recorder.
			 *
			 * @param pDevice The device pointer.
			 * @param usage The command recorder usage.
			 * @param bufferCount The backend primitive buffer count. Default is 1.
			 * @return The command recorder pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<CommandRecorder> createCommandRecorder(Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount = 1) = 0;

			/**
			 * Create a new index buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param type The buffer type.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Buffer> createBuffer(Device* pDevice, uint64_t size, BufferType type) = 0;

			/**
			 * Create a new image.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The image specification.
			 * @return The image pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Image> createImage(Device* pDevice, const ImageSpecification& specification) = 0;

			/**
			 * Create a new rasterizer.
			 *
			 * @param pDevice The device pointer.
			 * @param pCamera The camera which is used to render the scene.
			 * @param attachmentTypes The attachment types the render target should support.
			 * @param enableTripleBuffering Whether to enable triple-buffering. Default is false.
			 * @param multiSampleCount Multi-sampling count to use. Default is x1.
			 * @return The rasterizer pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Rasterizer> createRasterizer(Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering = false, MultiSamplingCount multiSampleCount = MultiSamplingCount::x1) = 0;

			/**
			 * Create a new swapchain.
			 *
			 * @param pDevice The device pointer.
			 * @param title The title of the window.
			 * @param width The window's width.
			 * @param height The window's height.
			 * @return The swapchain pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Swapchain> createSwapchain(Device* pDevice, const std::string& title, uint32_t width, uint32_t height) = 0;

			/**
			 * Create a new image view.
			 *
			 * @param pDevice The device pointer.
			 * @param pImage The image pointer.
			 * @param specification The view specification.
			 * @return The image view pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<ImageView> createImageView(Device* pDevice, Image* pImage, const ImageViewSpecification& specification) = 0;

			/**
			 * Create a new image sampler.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The sampler specification.
			 * @return The image sampler pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<ImageSampler> createImageSampler(Device* pDevice, const ImageSamplerSpecification& specification) = 0;

			/**
			 * Create a new rasterizing pipeline.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer.
			 * @param pRasterizer The rasterizer pointer.
			 * @param specification The pipeline specification.
			 * @return The pipeline pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<RasterizingPipeline> createRasterizingPipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, [[maybe_unused]] Rasterizer* pRasterizer, const RasterizingPipelineSpecification& specification) = 0;

			/**
			 * Create a new compute pipeline.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer. This can be null in which case the pipeline creation might get slow.
			 * @param computeShader The compute shader source.
			 * @return The pipeline pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<ComputePipeline> createComputePipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const ShaderSource& computeShader) = 0;
		};
	}
}