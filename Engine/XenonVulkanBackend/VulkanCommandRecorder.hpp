// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/CommandRecorder.hpp"

#include "VulkanCommandBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan command recorder class.
		 */
		class VulkanCommandRecorder final : public CommandRecorder, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param usage The command recorder usage.
			 * @param bufferCount The backend primitive buffer count. Default is 1.
			 */
			explicit VulkanCommandRecorder(VulkanDevice* pDevice, CommandRecorderUsage usage, uint32_t bufferCount = 1);

			/**
			 * Destructor.
			 */
			~VulkanCommandRecorder() override;

			/**
			 * Set the command recorder state to recording.
			 */
			void begin() override;

			/**
			 * Set the command recorder state to recording.
			 * This will set the command recorder's state to secondary usage (for multi-threading).
			 *
			 * @param pParent The parent command recorder pointer.
			 */
			void begin(CommandRecorder* pParent) override;

			/**
			 * Change the image layout of an image.
			 *
			 * @param image The image to change the layout of.
			 * @param currentLayout The current layout of the image.
			 * @param newLayout The new layout to change to.
			 * @param aspectFlags The image aspect flags.
			 * @param mipLevels The image mip levels. Default is 1.
			 * @param layers The image layers. Default is 1.
			 */
			void changeImageLayout(VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1, uint32_t layers = 1);

			/**
			 * Copy data from one buffer to another.
			 *
			 * @param pSource The source buffer to copy the data from.
			 * @param srcOffset The source buffer offset.
			 * @param pDestination The destination buffer to copy the data to.
			 * @param dstOffse The destination buffer offset.
			 * @param size The amount of data to copy in bytes.
			 */
			void copy(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size) override;

			/**
			 * Copy a source image to the swapchain.
			 *
			 * @param pSource The source image pointer.
			 * @param pDestination The destination swapchain.
			 */
			void copy(Image* pSource, Swapchain* pDestination) override;

			/**
			 * Bind a rasterizer to the command recorder.
			 *
			 * @param pRasterizer The rasterizer pointer.
			 * @param clearValues The rasterizer's clear values.
			 * @param usingSecondaryCommandRecorders Whether we are using secondary command recorders to bind the rasterizer's resources. Default is false.
			 */
			void bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues, bool usingSecondaryCommandRecorders = false) override;

			/**
			 * Bind a rasterizing pipeline to the command recorder.
			 *
			 * @param pPipeline The pipeline pointer.
			 * @param vertexSpecification The vertex specification.
			 */
			void bind(RasterizingPipeline* pPipeline, const VertexSpecification& vertexSpecification) override;

			/**
			 * Bind a vertex and an index buffer if provided.
			 * If only the vertex buffer is provided, it'll draw using these vertex data.
			 * If both the vertex and index buffers are provided it'll use the index data to draw.
			 *
			 * @param pVertexBuffer The vertex buffer pointer.
			 * @param vertexStride The vertex stride.
			 * @param pIndexBuffer The index buffer pointer. Default is nullptr.
			 * @param indexStride The stride of a single index. Default is 0.
			 */
			void bind(Buffer* pVertexBuffer, uint32_t vertexStride, Buffer* pIndexBuffer = nullptr, uint8_t indexStride = 0) override;

			/**
			 * Bind descriptors to the command recorder.
			 * Note that the descriptors can be null in which case this call will be disregarded.
			 *
			 * @param pPipeline The pipeline to which the descriptors are bound to.
			 * @param pUserDefinedDescrptor The user defined descriptor.
			 * @param pMaterialDescriptor The material descriptor.
			 * @param pCameraDescriptor The camera descriptor. Default is nullptr.
			 */
			void bind(RasterizingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pCameraDescriptor) override;

			/**
			 * Draw using the bound index buffers.
			 *
			 * @param vertexOffset The vertex offset.
			 * @param indexOffset The index offset.
			 * @param indexCount The number of indices to draw.
			 * @param instanceCount The number of instances to draw. Default is 1.
			 * @param firstInstance The first instance position. Default is 0.
			 */
			void drawIndexed(uint64_t vertexOffset, uint64_t indexOffset, uint64_t indexCount, uint32_t instanceCount = 1, uint32_t firstInstance = 0) override;

			/**
			 * Execute all the child command recorders.
			 */
			void executeChildren() override;

			/**
			 * End the command recorder recording.
			 */
			void end() override;

			/**
			 * Rotate the command recorder and select the next command buffer primitive.
			 * This is only applicable when having multiple buffers internally.
			 */
			void next() override;

			/**
			 * Submit the recorded commands to the GPU.
			 *
			 * @param pSwapchain The swapchain pointer. This is needed when rendering images to a window. Default is nullptr.
			 */
			void submit(Swapchain* pSwapchain = nullptr) override;

			/**
			 * Wait till the commands that were recorded has been executed.
			 *
			 * @param timeout The time to wait till the commands are executed in milliseconds. Default is uint64_t max.
			 */
			void wait(uint64_t timeout = UINT64_MAX) override;

		private:
			VkCommandBufferInheritanceInfo m_InheritanceInfo = {};

			VkCommandPool m_SecondaryPool = VK_NULL_HANDLE;
			std::vector<VulkanCommandBuffer> m_CommandBuffers;
			std::vector<VkCommandBuffer> m_ChildCommandBuffers;
			VulkanCommandBuffer* m_pCurrentBuffer = nullptr;
		};
	}
}