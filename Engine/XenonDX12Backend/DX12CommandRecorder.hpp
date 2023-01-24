// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/CommandRecorder.hpp"

#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 command recorder class.
		 */
		class DX12CommandRecorder final : public CommandRecorder, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param usage The command recorder usage.
			 * @param bufferCount The backend primitive buffer count. Default is 1.
			 */
			explicit DX12CommandRecorder(DX12Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount = 1);

			/**
			 * Destructor.
			 */
			~DX12CommandRecorder() override;

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
			 * Reset the occlusion query.
			 *
			 * @param pOcclusionQuery The query to reset.
			 */
			void resetQuery(OcclusionQuery* pOcclusionQuery) override;

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
			 * Bind a ray tracing pipeline.
			 *
			 * @param pPipeline The pipeline to bind.
			 */
			void bind(RayTracingPipeline* pPipeline) override;

			/**
			 * Bind a compute pipeline.
			 *
			 * @param pPipeline The pipeline to bind.
			 */
			void bind(ComputePipeline* pPipeline) override;

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
			 * @param pPerGeometryDescriptor The per-geometry descriptor.
			 * @param pSceneDescriptor The scene descriptor. Default is nullptr.
			 */
			void bind(RasterizingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pPerGeometryDescriptor, Descriptor* pSceneDescriptor) override;

			/**
			 * Bind descriptors to the command recorder.
			 * Note that the descriptors can be null in which case this call will be disregarded.
			 *
			 * @param pPipeline The pipeline to which the descriptors are bound to.
			 * @param pUserDefinedDescrptor The user defined descriptor.
			 * @param pMaterialDescriptor The material descriptor.
			 * @param pPerGeometryDescriptor The per-geometry descriptor.
			 * @param pSceneDescriptor The scene descriptor. Default is nullptr.
			 */
			void bind(RayTracingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pPerGeometryDescriptor, Descriptor* pSceneDescriptor) override;

			/**
			 * Bind descriptors to the command recorder.
			 * Note that the descriptor can be null in which case this call will be disregarded.
			 *
			 * @param pPipeline The pipeline to bind.
			 * @param pUserDefinedDescriptor The user defined descriptor to bind.
			 */
			void bind(ComputePipeline* pPipeline, Descriptor* pUserDefinedDescriptor) override;

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
			 * Begin the occlusion query.
			 *
			 * @param pOcclusionQuery The occlusion query to start recording.
			 * @param index The sample index.
			 */
			void beginQuery(OcclusionQuery* pOcclusionQuery, uint32_t index) override;

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
			 * Draw the scene using ray tracing.
			 *
			 * @param pRayTracer The ray tracer where the output will be written to.
			 * @param pShaderBindingTable The shader binding table.
			 */
			void drawRayTraced(RayTracer* pRayTracer, ShaderBindingTable* pShaderBindingTable) override;

			/**
			 * Perform compute operations.
			 *
			 * @param width The cluster width.
			 * @param height The cluster height.
			 * @param depth The cluster depth.
			 */
			void compute(uint32_t width, uint32_t height, uint32_t depth) override;

			/**
			 * End the occlusion query.
			 *
			 * @param pOcclusionQuery The occlusion query to end recording.
			 * @param index The sample index.
			 */
			void endQuery(OcclusionQuery* pOcclusionQuery, uint32_t index) override;

			/**
			 * Execute a child (secondary) command recorder.
			 *
			 * @param pChildRecorder The child command recorder.
			 * @param pActivePipeline The active pipeline of the child recorder.
			 */
			void executeChild(CommandRecorder* pChildRecorder, RasterizingPipeline* pActivePipeline) override;

			/**
			 * Execute a child (secondary) command recorder.
			 *
			 * @param pChildRecorder The child command recorder.
			 * @param pActivePipeline The active pipeline of the child recorder.
			 */
			void executeChild(CommandRecorder* pChildRecorder, RayTracingPipeline* pActivePipeline) override;

			/**
			 * Get the query results from the command recorder.
			 *
			 * @param pOcclusionQuery The occlusion query pointer.
			 */
			void getQueryResults(OcclusionQuery* pOcclusionQuery) override;

			/**
			 * Build the acceleration structure.
			 *
			 * @param desc The acceleration structure description.
			 */
			void buildAccelerationStructure(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& desc);

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
			void submit(Swapchain* pSawpchain = nullptr) override;

			/**
			 * Wait till the commands that were recorded has been executed.
			 *
			 * @param timeout The time to wait till the commands are executed in milliseconds. Default is uint64_t max.
			 */
			void wait(uint64_t timeout = UINT64_MAX) override;

		public:
			/**
			 * Get the current command list.
			 *
			 * @return The command list pointer.
			 */
			[[nodiscard]] ID3D12GraphicsCommandList* getCurrentCommandList() noexcept { return m_pCurrentCommandList; }

			/**
			 * Get the current command list.
			 *
			 * @return The command list pointer.
			 */
			[[nodiscard]] const ID3D12GraphicsCommandList* getCurrentCommandList() const noexcept { return m_pCurrentCommandList; }

		private:
			std::mutex m_Mutex;

			ComPtr<ID3D12CommandAllocator> m_CommandAllocator;

			std::vector<ComPtr<ID3D12GraphicsCommandList5 >> m_pCommandLists;
			std::vector<ComPtr<ID3D12Fence>> m_pCommandListFences;

			ID3D12GraphicsCommandList5* m_pCurrentCommandList = nullptr;
			ID3D12Fence* m_pCurrentCommandListFence = nullptr;

			DX12CommandRecorder* m_pParentCommandRecorder = nullptr;

			bool m_IsRecording = false;
		};
	}
}