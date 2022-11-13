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
		class VulkanCommandRecorder final : public VulkanDeviceBoundObject, public CommandRecorder
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
			~VulkanCommandRecorder() override = default;

			/**
			 * Set the command recorder state to recording.
			 */
			void begin() override;

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
			 */
			void bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues) override;

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
			 * @param timeout The time to wait till the commands are executed. Default is uint64_t max.
			 */
			void wait(uint64_t timeout = std::numeric_limits<uint64_t>::max()) override;

		private:
			std::vector<VulkanCommandBuffer> m_CommandBuffers;
			VulkanCommandBuffer* m_pCurrentBuffer = nullptr;
		};
	}
}