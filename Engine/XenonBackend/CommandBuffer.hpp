// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferAllocator.hpp"
#include "Buffer.hpp"
#include "Swapchain.hpp"
#include "RasterizingPipeline.hpp"
#include "Descriptor.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Index buffer stride enum.
		 */
		enum class IndexBufferStride : uint8_t
		{
			Uint16 = sizeof(uint16_t),
			Uint32 = sizeof(uint32_t)
		};

		/**
		 * Command buffer class.
		 * This class is used to send commands to the GPU to be executed.
		 */
		class CommandBuffer : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pAllocator The allocator pointer.
			 */
			explicit CommandBuffer([[maybe_unused]] const Device* pDevice, [[maybe_unused]] const CommandBufferAllocator* pAllocator) {}

			/**
			 * Move constructor.
			 *
			 * @param other The other command buffer.
			 */
			CommandBuffer(CommandBuffer&& other) noexcept : m_bIsRenderTargetBound(std::exchange(other.m_bIsRenderTargetBound, false)) {}

			/**
			 * Set the command buffer state to recording.
			 */
			virtual void begin() = 0;

			/**
			 * Set the command buffer state to recording.
			 * This will set the command buffer's state to secondary usage (for multi-threading).
			 *
			 * @param pParent The parent command buffer pointer.
			 */
			virtual void begin(CommandBuffer* pParent) = 0;

			/**
			 * Copy data from one buffer to another.
			 *
			 * @param pSource The source buffer to copy the data from.
			 * @param srcOffset The source buffer offset.
			 * @param pDestination The destination buffer to copy the data to.
			 * @param dstOffse The destination buffer offset.
			 * @param size The amount of data to copy in bytes.
			 */
			virtual void copy(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size) = 0;

			/**
			 * Copy a source image to the swapchain.
			 *
			 * @param pSource The source image pointer.
			 * @param pDestination The destination swapchain.
			 */
			virtual void copy(Image* pSource, Swapchain* pDestination) = 0;

			/**
			 * Copy a source image to the destination image.
			 *
			 * @param pSource The source image pointer.
			 * @param sourceOffset The source image's offset.
			 * @param pDestination The destination image pointer.
			 * @param destinationOffset The destination image's offset.
			 */
			virtual void copy(Image* pSource, const glm::vec3& sourceOffset, Image* pDestination, const glm::vec3& destinationOffset) = 0;

			/**
			 * Copy image data from a buffer to an image.
			 *
			 * @param pSource The source buffer pointer.
			 * @param bufferOffset The buffer offset.
			 * @param pImage the image pointer to copy the data to.
			 * @param imageSize The image data size to copy.
			 * @param imageOffset The image offset to copy the data to. Default is 0.
			 */
			virtual void copy(Buffer* pSource, uint64_t bufferOffset, Image* pImage, glm::vec3 imageSize, glm::vec3 imageOffset = glm::vec3(0)) = 0;

			/**
			 * Bind a rasterizer to the command buffer.
			 *
			 * @param pRasterizer The rasterizer pointer.
			 * @param clearValues The rasterizer's clear values.
			 * @param usingSecondaryCommandbuffers Whether we are using secondary command buffers to bind the rasterizer's resources. Default is false.
			 */
			virtual void bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues, bool usingSecondaryCommandbuffers = false) = 0;

			/**
			 * Bind a rasterizing pipeline to the command buffer.
			 *
			 * @param pPipeline The pipeline pointer.
			 * @param vertexSpecification The vertex specification.
			 */
			virtual void bind(RasterizingPipeline* pPipeline, const VertexSpecification& vertexSpecification) = 0;

			/**
			 * Bind a vertex buffer to the command buffer.
			 *
			 * @param pVertexBuffer The vertex buffer pointer.
			 * @param vertexStride The vertex stride.
			 */
			virtual void bind(Buffer* pVertexBuffer, uint32_t vertexStride) = 0;

			/**
			 * Bind an index buffer to the command buffer.
			 *
			 * @param pIndexBuffer The index buffer pointer.
			 * @param indexStride The stride of a single index.
			 */
			virtual void bind(Buffer* pIndexBuffer, IndexBufferStride indexStride) = 0;

			/**
			 * Bind descriptors to the command buffer.
			 * Note that the descriptors can be null in which case this call will be disregarded.
			 *
			 * @param pPipeline The pipeline to which the descriptors are bound to.
			 * @param pUserDefinedDescrptor The user defined descriptor.
			 * @param pMaterialDescriptor The material descriptor.
			 * @param pCameraDescriptor The camera descriptor. Default is nullptr.
			 */
			virtual void bind(RasterizingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pCameraDescriptor) = 0;

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
			virtual void setViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;

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
			virtual void setViewportNatural(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;

			/**
			 * Set the scissor to draw.
			 *
			 * @param x The x offset.
			 * @param y The y offset.
			 * @param width The width to scissor.
			 * @param height The height to scissor.
			 */
			virtual void setScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) = 0;

			/**
			 * Draw using the bound index buffers.
			 *
			 * @param vertexOffset The vertex offset.
			 * @param indexOffset The index offset.
			 * @param indexCount The number of indices to draw.
			 * @param instanceCount The number of instances to draw. Default is 1.
			 * @param firstInstance The first instance position. Default is 0.
			 */
			virtual void drawIndexed(uint64_t vertexOffset, uint64_t indexOffset, uint64_t indexCount, uint32_t instanceCount = 1, uint32_t firstInstance = 0) = 0;

			/**
			 * Execute all the child command buffers.
			 */
			virtual void executeChildren() = 0;

			/**
			 * End the command buffer recording.
			 */
			virtual void end() = 0;

			/**
			 * Submit the recorded commands to the GPU.
			 *
			 * @param pSwapchain The swapchain pointer. This is needed when rendering images to a window. Default is nullptr.
			 */
			virtual void submit(Swapchain* pSawpchain = nullptr) = 0;

			/**
			 * Wait till the commands that were recorded has been executed.
			 *
			 * @param timeout The time to wait till the commands are executed in milliseconds. Default is uint64_t max.
			 */
			virtual void wait(uint64_t timeout = UINT64_MAX) = 0;

			/**
			 * Move assign operator.
			 *
			 * @param other The other command buffer.
			 * @return The current command buffer reference.
			 */
			CommandBuffer& operator=([[maybe_unused]] CommandBuffer&& other) noexcept
			{
				m_bIsRenderTargetBound = std::exchange(other.m_bIsRenderTargetBound, false);

				return *this;
			}

		protected:
			bool m_bIsRenderTargetBound = false;
		};
	}
}