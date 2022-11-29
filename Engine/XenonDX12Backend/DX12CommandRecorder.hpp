// Copyright 2022 Dhiraj Wishal
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
			~DX12CommandRecorder() override = default;

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
			void submit(Swapchain* pSawpchain = nullptr) override;

			/**
			 * Wait till the commands that were recorded has been executed.
			 *
			 * @param timeout The time to wait till the commands are executed in milliseconds. Default is uint64_t max.
			 */
			void wait(uint64_t timeout = UINT64_MAX) override;

		private:
			DX12Device* m_pDevice = nullptr;
			std::vector<ComPtr<ID3D12CommandAllocator>> m_pCommandAllocators;
			std::vector<ComPtr<ID3D12GraphicsCommandList>> m_pCommandLists;
			std::vector<ComPtr<ID3D12Fence>> m_pCommandListFences;

			ID3D12CommandAllocator* m_pCurrentCommandAllocator = nullptr;
			ID3D12GraphicsCommandList* m_pCurrentCommandList = nullptr;
			ID3D12Fence* m_pCurrentCommandListFence = nullptr;
		};
	}
}