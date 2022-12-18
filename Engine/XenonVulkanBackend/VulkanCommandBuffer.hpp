// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/CommandBuffer.hpp"

#include "VulkanCommandBufferAllocator.hpp"

#ifdef max
#undef max

#endif

namespace Xenon
{
	namespace Backend
	{
		class VulkanSwapchain;

		/**
		 * Vulkan command buffer structure.
		 * This contains the actual Vulkan command buffer and it's synchronization primitives.
		 */
		class VulkanCommandBuffer final : public CommandBuffer, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer to which the command buffer belongs to.
			 * @param pAllocator The command buffer allocator pointer.
			 * @param buffer The allocated command buffer.
			 */
			explicit VulkanCommandBuffer(VulkanDevice* pDevice, VulkanCommandBufferAllocator* pAllocator, VkCommandBuffer buffer);

			/**
			 * Move constructor.
			 *
			 * @param other The other command buffer.
			 */
			VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept;

			/**
			 * Destructor.
			 */
			~VulkanCommandBuffer() override;

			/**
			 * Set the command recorder state to recording.
			 */
			void begin() override;

			/**
			 * Set the command buffer state to recording.
			 * This will set the command buffer's state to secondary usage (for multi-threading).
			 *
			 * @param pParent The parent command buffer pointer.
			 */
			void begin(CommandBuffer* pParent) override;

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
			 * Copy a source image to the destination image.
			 *
			 * @param pSource The source image pointer.
			 * @param sourceOffset The source image's offset.
			 * @param pDestination The destination image pointer.
			 * @param destinationOffset The destination image's offset.
			 */
			void copy(Image* pSource, const glm::vec3& sourceOffset, Image* pDestination, const glm::vec3& destinationOffset) override;

			/**
			 * Copy image data from a buffer to an image.
			 *
			 * @param pSource The source buffer pointer.
			 * @param bufferOffset The buffer offset.
			 * @param pImage the image pointer to copy the data to.
			 * @param imageSize The image data size to copy.
			 * @param imageOffset The image offset to copy the data to. Default is 0.
			 */
			void copy(Buffer* pSource, uint64_t bufferOffset, Image* pImage, glm::vec3 imageSize, glm::vec3 imageOffset = glm::vec3(0)) override;

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
			 * Bind a vertex buffer to the command recorder.
			 *
			 * @param pVertexBuffer The vertex buffer pointer.
			 * @param vertexStride The vertex stride.
			 */
			void bind(Buffer* pVertexBuffer, uint32_t vertexStride) override;

			/**
			 * Bind an index buffer to the command recorder.
			 *
			 * @param pIndexBuffer The index buffer pointer.
			 * @param indexStride The stride of a single index.
			 */
			void bind(Buffer* pIndexBuffer, IndexBufferStride indexStride) override;

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
			 * Set the viewport.
			 *
			 * @param x The x offset.
			 * @param y The y offset.
			 * @param width The width to viewport.
			 * @param height The height to viewport.
			 * @param minDepth The minimum depth.
			 * @param maxDepth The maximum depth.
			 */
			void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;

			/**
			 * Set the natural viewport.
			 * Vulkan uses the normal viewport type here whereas the DirectX 12 backend will flip the viewport vertically since it's coordinate system is different to Vulkan.
			 * Use this if Vulkan renders the scene properly and in DirectX 12 the view is flipped vertically (upside-down).
			 *
			 * @param x The x offset.
			 * @param y The y offset.
			 * @param width The width to viewport.
			 * @param height The height to viewport.
			 * @param minDepth The minimum depth.
			 * @param maxDepth The maximum depth.
			 */
			void setViewportNatural(float x, float y, float width, float height, float minDepth, float maxDepth) override;

			/**
			 * Set the scissor to draw.
			 *
			 * @param x The x offset.
			 * @param y The y offset.
			 * @param width The width to scissor.
			 * @param height The height to scissor.
			 */
			void setScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) override;

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

			/**
			 * This will block the thread and wait till the command buffer has finished it's execution.
			 *
			 * @param timeout The timeout time to wait for. Default is the uint64_t max.
			 */
			void wait(uint64_t timeout = std::numeric_limits<uint64_t>::max());

			/**
			 * Submit the command buffer to the device.
			 *
			 * @param pipelineStageFlags The pipeline stage flags.
			 * @param queue The queue to submit to.
			 * @param pSwapchain The swapchain to get the semaphores from. Default is nullptr.
			 */
			void submit(VkPipelineStageFlags pipelineStageFlags, VkQueue queue, VulkanSwapchain* pSwapchain = nullptr);

		public:
			/**
			 * Move assignment operator.
			 *
			 * @param other The other command buffer.
			 * @return The moved buffer reference.
			 */
			VulkanCommandBuffer& operator=(VulkanCommandBuffer&& other) noexcept;

			/**
			 * VkCommandBuffer operator.
			 * This can be used to conveniently get the Vulkan command buffer handle.
			 *
			 * @return The Vulkan command buffer.
			 */
			operator VkCommandBuffer() const { return m_CommandBuffer; }

			/**
			 * VkFence operator.
			 * This can be used to conveniently get the Vulkan fence handle.
			 *
			 * @return The Vulkan fence.
			 */
			operator VkFence() const { return m_Fence; }

		public:
			/**
			 * Get the command buffer.
			 *
			 * @return The command buffer.
			 */
			[[nodiscard]] VkCommandBuffer getCommandBuffer() const { return m_CommandBuffer; }

			/**
			 * Get the command buffer address (pointer).
			 *
			 * @return The const command buffer pointer.
			 */
			[[nodiscard]] const VkCommandBuffer* getCommandBufferAddress() const { return &m_CommandBuffer; }

		private:
			/**
			 * Call a provided method synchronously to the command pool.
			 *
			 * @tparam Function The function to call.
			 * @tparam Arguments The argument types to forward to the function.
			 * @param function The function to call.
			 * @param arguments The arguments to forward.
			 */
			template<class Function, class... Arguments>
			void issueCall(const Function& function, Arguments&&... arguments)
			{
				getCommandPool().access([this, function]([[maybe_unused]] const VkCommandPool& pool, Arguments&&... args)
					{
						function(std::forward<Arguments>(args)...);
					}
				, std::forward<Arguments>(arguments)...);
			}

		private:
			VkCommandBufferInheritanceInfo m_InheritanceInfo = {};

			std::vector<VkCommandBuffer> m_ChildCommandBuffers;

			VulkanCommandBufferAllocator* m_pCommandAllocator = nullptr;

			VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

			VkSemaphore m_WaitSemaphore = VK_NULL_HANDLE;
			VkSemaphore m_SignalSemaphore = VK_NULL_HANDLE;

			VkFence m_Fence = VK_NULL_HANDLE;
			bool m_IsFenceFree = true;
		};
	}
}