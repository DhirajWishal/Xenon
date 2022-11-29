// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "Swapchain.hpp"
#include "RasterizingPipeline.hpp"
#include "Descriptor.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Command recorder usage enum.
		 */
		enum class CommandRecorderUsage : uint8_t
		{
			Compute = XENON_BIT_SHIFT(0),
			Graphics = XENON_BIT_SHIFT(1),
			Transfer = XENON_BIT_SHIFT(2),
			Secondary = XENON_BIT_SHIFT(3)
		};

		XENON_DEFINE_ENUM_OR(CommandRecorderUsage);
		XENON_DEFINE_ENUM_AND(CommandRecorderUsage);

		/**
		 * Command recorder class.
		 * This is used to record and submit commands to the GPU and execute them.
		 */
		class CommandRecorder : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param usage The command recorder usage.
			 * @param bufferCount The backend primitive buffer count. Default is 1.
			 */
			explicit CommandRecorder([[maybe_unused]] const Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount = 1) : m_BufferCount(bufferCount), m_Usage(usage) {}

			/**
			 * Set the command recorder state to recording.
			 */
			virtual void begin() = 0;

			/**
			 * Set the command recorder state to recording.
			 * This will set the command recorder's state to secondary usage (for multi-threading).
			 *
			 * @param pParent The parent command recorder pointer.
			 */
			virtual void begin(CommandRecorder* pParent) = 0;

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
			 * Bind a rasterizer to the command recorder.
			 *
			 * @param pRasterizer The rasterizer pointer.
			 * @param clearValues The rasterizer's clear values.
			 * @param usingSecondaryCommandRecorders Whether we are using secondary command recorders to bind the rasterizer's resources. Default is false.
			 */
			virtual void bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues, bool usingSecondaryCommandRecorders = false) = 0;

			/**
			 * Bind a rasterizing pipeline to the command recorder.
			 *
			 * @param pPipeline The pipeline pointer.
			 * @param vertexSpecification The vertex specification.
			 */
			virtual void bind(RasterizingPipeline* pPipeline, const VertexSpecification& vertexSpecification) = 0;

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
			virtual void bind(Buffer* pVertexBuffer, uint32_t vertexStride, Buffer* pIndexBuffer = nullptr, uint8_t indexStride = 0) = 0;

			/**
			 * Bind descriptors to the command recorder.
			 * Note that the descriptors can be null in which case this call will be disregarded.
			 *
			 * @param pPipeline The pipeline to which the descriptors are bound to.
			 * @param pUserDefinedDescrptor The user defined descriptor.
			 * @param pMaterialDescriptor The material descriptor.
			 * @param pCameraDescriptor The camera descriptor. Default is nullptr.
			 */
			virtual void bind(RasterizingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pCameraDescriptor) = 0;

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
			 * Execute all the child command recorders.
			 */
			virtual void executeChildren() = 0;

			/**
			 * End the command recorder recording.
			 */
			virtual void end() = 0;

			/**
			 * Rotate the command recorder and select the next command buffer primitive.
			 * This is only applicable when having multiple buffers internally.
			 */
			virtual void next() = 0;

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

		public:
			/**
			 * Get the backend buffer count.
			 *
			 * @return The buffer count.
			 */
			[[nodiscard]] uint32_t getBufferCount() const { return m_BufferCount; }

		protected:
			/**
			 * Increment the current buffer index.
			 *
			 * @return The incremented index.
			 */
			[[nodiscard]] uint32_t incrementIndex() { return m_CurrentIndex = ++m_CurrentIndex % m_BufferCount; }

		protected:
			uint32_t m_BufferCount;
			uint32_t m_CurrentIndex = 0;
			CommandRecorderUsage m_Usage = CommandRecorderUsage::Transfer;

			bool m_IsRenderTargetBound = false;
		};
	}
}