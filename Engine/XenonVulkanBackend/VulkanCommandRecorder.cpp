// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandRecorder.hpp"
#include "VulkanMacros.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanRasterizer.hpp"
#include "VulkanRasterizingPipeline.hpp"

namespace /* anonymous */
{
	/**
	 * Get the Vulkan clear values.
	 *
	 * @param attachmenTYpes The attachment types used by the rasterizer.
	 * @param clearValues The clear values to clear the rasterizer.
	 * @return The Vulkan clear values.
	 */
	std::vector<VkClearValue> GetClearValues(Xenon::Backend::AttachmentType attachmentTypes, const std::vector<Xenon::Backend::Rasterizer::ClearValueType>& clearValues)
	{
		auto itr = clearValues.begin();

		std::vector<VkClearValue> vkClearValues;
		if (attachmentTypes & Xenon::Backend::AttachmentType::Color)
		{
			const auto& clearColor = std::get<glm::vec4>(*itr);

			auto& clearValue = vkClearValues.emplace_back();
			clearValue.color.float32[0] = clearColor.x;
			clearValue.color.float32[1] = clearColor.y;
			clearValue.color.float32[2] = clearColor.z;
			clearValue.color.float32[3] = clearColor.w;

			++itr;
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::EntityID)
		{
			const auto& clearColor = std::get<glm::vec3>(*itr);

			auto& clearValue = vkClearValues.emplace_back();
			clearValue.color.float32[0] = clearColor.x;
			clearValue.color.float32[1] = clearColor.y;
			clearValue.color.float32[2] = clearColor.z;
			clearValue.color.float32[3] = 0.0f;

			++itr;
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::Normal)
		{
			const auto clearColor = std::get<float>(*itr);

			auto& clearValue = vkClearValues.emplace_back();
			clearValue.color.float32[0] = clearColor;
			clearValue.color.float32[1] = 0.0f;
			clearValue.color.float32[2] = 0.0f;
			clearValue.color.float32[3] = 0.0f;

			++itr;
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::Depth)
		{
			vkClearValues.emplace_back().depthStencil.depth = std::get<float>(*itr);
			++itr;
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::Stencil)
		{
			vkClearValues.emplace_back().depthStencil.stencil = std::get<uint32_t>(*itr);
			++itr;
		}

		return vkClearValues;
	}

	/**
	 * Get the pipeline stage flags from access flags.
	 *
	 * @param flags Access flags.
	 * @return The stage flags.
	 */
	[[nodiscard]] constexpr VkPipelineStageFlags GetPipelineStageFlags(VkAccessFlags flags) noexcept
	{
		switch (flags)
		{
		case VK_ACCESS_INDIRECT_COMMAND_READ_BIT:
			return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
				| VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

		case VK_ACCESS_INDEX_READ_BIT:
			return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

		case VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT:
			return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

		case VK_ACCESS_UNIFORM_READ_BIT:
			return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR
				| VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
				| VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
				| VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
				| VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
				| VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				| VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

		case VK_ACCESS_SHADER_READ_BIT:
			return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR
				| VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR
				| VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
				| VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
				| VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
				| VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
				| VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				| VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

		case VK_ACCESS_SHADER_WRITE_BIT:
			return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR
				| VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
				| VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
				| VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
				| VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
				| VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				| VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

		case VK_ACCESS_INPUT_ATTACHMENT_READ_BIT:
			return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		case VK_ACCESS_COLOR_ATTACHMENT_READ_BIT:
			return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		case VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT:
			return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT:
			return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
				| VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

		case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:
			return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
				| VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

		case VK_ACCESS_TRANSFER_READ_BIT:
			return VK_PIPELINE_STAGE_TRANSFER_BIT
				| VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

		case VK_ACCESS_TRANSFER_WRITE_BIT:
			return VK_PIPELINE_STAGE_TRANSFER_BIT
				| VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

		case VK_ACCESS_HOST_READ_BIT:
			return VK_PIPELINE_STAGE_HOST_BIT;

		case VK_ACCESS_HOST_WRITE_BIT:
			return VK_PIPELINE_STAGE_HOST_BIT;

		case VK_ACCESS_MEMORY_READ_BIT:
			return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		case VK_ACCESS_MEMORY_WRITE_BIT:
			return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		case VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT:
			return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		case VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV:
			return VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;

		case VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV:
			return VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;

		case VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT:
			return VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;

		case VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR:
			return VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

		case VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT:
			return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;

		case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT:
			return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;

		case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT:
			return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT
				| VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

		case VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR:
			return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
				| VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
				| VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
				| VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
				| VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				| VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
				| VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR
				| VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

		case VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR:
			return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

		case VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT:
			return VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;

		default:
			return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanCommandRecorder::VulkanCommandRecorder(VulkanDevice* pDevice, CommandRecorderUsage usage, uint32_t bufferCount /*= 1*/)
			: CommandRecorder(pDevice, usage, bufferCount)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Allocate the command buffers.
			auto function = [this, bufferCount](VkCommandPool commandPool)
			{
				VkCommandBufferAllocateInfo allocateInfo = {};
				allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocateInfo.pNext = nullptr;
				allocateInfo.commandPool = commandPool;
				allocateInfo.level = m_Usage & CommandRecorderUsage::Secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocateInfo.commandBufferCount = bufferCount;

				std::vector<VkCommandBuffer> commandBuffers(bufferCount);
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkAllocateCommandBuffers(m_pDevice->getLogicalDevice(), &allocateInfo, commandBuffers.data()), "Failed to allocate command buffers!");

				// Create the command buffers.
				for (uint32_t i = 0; i < bufferCount; i++)
					m_CommandBuffers.emplace_back(m_pDevice, commandBuffers[i], commandPool);

				// Select the default buffer.
				m_pCurrentBuffer = &m_CommandBuffers[m_CurrentIndex];
			};

			// Get the command pool from the device and create the buffers.
			switch (usage)
			{
			case Xenon::Backend::CommandRecorderUsage::Compute:
				pDevice->getComputeCommandPool().access(std::move(function));
				break;

			case Xenon::Backend::CommandRecorderUsage::Graphics:
				pDevice->getGraphicsCommandPool().access(std::move(function));
				break;

			case Xenon::Backend::CommandRecorderUsage::Transfer:
				pDevice->getTransferCommandPool().access(std::move(function));
				break;

			case Xenon::Backend::CommandRecorderUsage::Secondary:
				pDevice->getGraphicsCommandPool().access(std::move(function));
				break;

			default:
				XENON_LOG_FATAL("Invalid command recorder usage!");
			}

			// Setup the initial inheritance info structure data.
			m_InheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			m_InheritanceInfo.pNext = nullptr;
			m_InheritanceInfo.subpass = 0;
			m_InheritanceInfo.occlusionQueryEnable = VK_FALSE;
			m_InheritanceInfo.queryFlags = 0;
			m_InheritanceInfo.pipelineStatistics = 0;
		}

		void VulkanCommandRecorder::begin()
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = VK_NULL_HANDLE;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			m_pCurrentBuffer->wait();
			m_pDevice->getDeviceTable().vkBeginCommandBuffer(*m_pCurrentBuffer, &beginInfo);
		}

		void VulkanCommandRecorder::begin(CommandRecorder* pParent)
		{
			auto pVkParent = pParent->as<VulkanCommandRecorder>();

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = VK_NULL_HANDLE;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			beginInfo.pInheritanceInfo = &pVkParent->m_InheritanceInfo;

			m_pCurrentBuffer->wait();
			m_pDevice->getDeviceTable().vkBeginCommandBuffer(*m_pCurrentBuffer, &beginInfo);

			// Insert he child (this) command buffer.
			pVkParent->m_ChildCommandBuffers.emplace_back(m_pCurrentBuffer->getCommandBuffer());
		}

		void VulkanCommandRecorder::changeImageLayout(VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, uint32_t mipLevels /*= 1*/, uint32_t layers /*= 1*/)
		{
			// Unbind the previous render pass if we need to.
			if (m_IsRenderTargetBound)
			{
				m_IsRenderTargetBound = false;
				m_pDevice->getDeviceTable().vkCmdEndRenderPass(*m_pCurrentBuffer);
			}

			// Create the memory barrier.
			VkImageMemoryBarrier memorybarrier = {};
			memorybarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memorybarrier.srcAccessMask = 0;
			memorybarrier.dstAccessMask = 0;
			memorybarrier.oldLayout = currentLayout;
			memorybarrier.newLayout = newLayout;
			memorybarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memorybarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memorybarrier.image = image;
			memorybarrier.subresourceRange.aspectMask = aspectFlags;
			memorybarrier.subresourceRange.baseMipLevel = 0;
			memorybarrier.subresourceRange.levelCount = mipLevels;
			memorybarrier.subresourceRange.baseArrayLayer = 0;
			memorybarrier.subresourceRange.layerCount = layers;

			// Resolve the source access masks.
			switch (currentLayout)
			{
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_UNDEFINED:
				memorybarrier.srcAccessMask = 0;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				memorybarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				//vMB.srcAccessMask = VK_ACCESS_;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				memorybarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			default:
				XENON_LOG_FATAL("Unsupported layout transition!");
				return;
			}

			// Resolve the destination access masks.
			switch (newLayout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				memorybarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				memorybarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				memorybarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				memorybarrier.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				memorybarrier.srcAccessMask |= VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				memorybarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			default:
				XENON_LOG_FATAL("Unsupported layout transition!");
				return;
			}

			// Issue the commands. 
			m_pDevice->getDeviceTable().vkCmdPipelineBarrier(
				*m_pCurrentBuffer,
				GetPipelineStageFlags(memorybarrier.srcAccessMask),
				GetPipelineStageFlags(memorybarrier.dstAccessMask),
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&memorybarrier
			);
		}

		void VulkanCommandRecorder::copy(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size)
		{
			VkBufferCopy bufferCopy = {};
			bufferCopy.size = size;
			bufferCopy.srcOffset = srcOffset;
			bufferCopy.dstOffset = dstOffset;

			m_pDevice->getDeviceTable().vkCmdCopyBuffer(*m_pCurrentBuffer, pSource->as<VulkanBuffer>()->getBuffer(), pDestination->as<VulkanBuffer>()->getBuffer(), 1, &bufferCopy);
		}

		void VulkanCommandRecorder::copy(Image* pSource, Swapchain* pDestination)
		{
			auto pVkImage = pSource->as<VulkanImage>();
			auto pVkSwapchain = pDestination->as<VulkanSwapchain>();

			// Return if we're minimized.
			if (!pVkSwapchain->isRenderable())
				return;

			const auto currentSwapchainImage = pVkSwapchain->getCurrentImage();

			VkImageBlit blit = {};
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.srcSubresource.mipLevel = 0;
			blit.srcOffsets[0].z = blit.srcOffsets[0].y = blit.srcOffsets[0].x = 0;
			blit.srcOffsets[1].x = static_cast<int32_t>(pVkImage->getWidth());
			blit.srcOffsets[1].y = static_cast<int32_t>(pVkImage->getHeight());
			blit.srcOffsets[1].z = 1;
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;
			blit.dstSubresource.mipLevel = 0;
			blit.dstOffsets[0].z = blit.dstOffsets[0].y = blit.dstOffsets[0].x = 0;
			blit.dstOffsets[1].x = static_cast<int32_t>(pVkSwapchain->getRenderWidth());
			blit.dstOffsets[1].y = static_cast<int32_t>(pVkSwapchain->getRenderHeight());
			blit.dstOffsets[1].z = 1;

			// Prepare to transfer.
			changeImageLayout(pVkImage->getImage(), pVkImage->getImageLayout(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, blit.srcSubresource.aspectMask);
			changeImageLayout(currentSwapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

			// Copy the image.
			m_pDevice->getDeviceTable().vkCmdBlitImage(*m_pCurrentBuffer, pVkImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, currentSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

			// Change back to previous.
			changeImageLayout(pVkImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pVkImage->getImageLayout(), blit.srcSubresource.aspectMask);
			changeImageLayout(currentSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		void VulkanCommandRecorder::bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues, bool usingSecondaryCommandRecorders /*= false*/)
		{
			// Unbind the previous render pass if we need to.
			if (m_IsRenderTargetBound)
				m_pDevice->getDeviceTable().vkCmdEndRenderPass(*m_pCurrentBuffer);

			// Cast the Vulkan rasterizer and get the clear values.
			auto pVkRenderPass = pRasterizer->as<VulkanRasterizer>();
			const auto vkClearValues = GetClearValues(pVkRenderPass->getAttachmentTypes(), clearValues);

			// Begin the render pass.
			VkRenderPassBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.pNext = nullptr;
			beginInfo.renderPass = pVkRenderPass->getRenderPass();
			beginInfo.framebuffer = pVkRenderPass->getFramebuffer();
			beginInfo.renderArea.extent.width = pVkRenderPass->getCamera()->getWidth();
			beginInfo.renderArea.extent.height = pVkRenderPass->getCamera()->getHeight();
			beginInfo.renderArea.offset.x = 0.0f;
			beginInfo.renderArea.offset.y = 0.0f;
			beginInfo.clearValueCount = static_cast<uint32_t>(vkClearValues.size());
			beginInfo.pClearValues = vkClearValues.data();

			const auto contents = usingSecondaryCommandRecorders ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS : VK_SUBPASS_CONTENTS_INLINE;
			m_pDevice->getDeviceTable().vkCmdBeginRenderPass(*m_pCurrentBuffer, &beginInfo, contents);
			m_IsRenderTargetBound = true;

			// Setup the inheritance info.
			m_InheritanceInfo.renderPass = beginInfo.renderPass;
			m_InheritanceInfo.framebuffer = beginInfo.framebuffer;
		}

		void VulkanCommandRecorder::bind(RasterizingPipeline* pPipeline, const VertexSpecification& vertexSpecification)
		{
			m_pDevice->getDeviceTable().vkCmdBindPipeline(*m_pCurrentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->as<VulkanRasterizingPipeline>()->getPipeline(vertexSpecification).m_Pipeline);
		}

		void VulkanCommandRecorder::executeChildren()
		{
			// Skip if we don't have any children :(
			if (m_ChildCommandBuffers.empty())
				return;

			m_pDevice->getDeviceTable().vkCmdExecuteCommands(*m_pCurrentBuffer, static_cast<uint32_t>(m_ChildCommandBuffers.size()), m_ChildCommandBuffers.data());
			m_ChildCommandBuffers.clear();
		}

		void VulkanCommandRecorder::end()
		{
			// Unbind the previous render pass if we need to.
			if (m_IsRenderTargetBound)
			{
				m_IsRenderTargetBound = false;
				m_pDevice->getDeviceTable().vkCmdEndRenderPass(*m_pCurrentBuffer);
			}

			m_pDevice->getDeviceTable().vkEndCommandBuffer(*m_pCurrentBuffer);
		}

		void VulkanCommandRecorder::next()
		{
			m_pCurrentBuffer = &m_CommandBuffers[incrementIndex()];
		}

		void VulkanCommandRecorder::submit(Swapchain* pSwapchain /*= nullptr*/)
		{
			switch (m_Usage)
			{
			case Xenon::Backend::CommandRecorderUsage::Compute:
				m_pDevice->getComputeQueue().access([this](const VulkanQueue& queue) { m_pCurrentBuffer->submit(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, queue.getQueue()); });
				break;

			case Xenon::Backend::CommandRecorderUsage::Graphics:
				m_pDevice->getGraphicsQueue().access([this](const VulkanQueue& queue, Swapchain* pSwapchain)
					{
						m_pCurrentBuffer->submit(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, queue.getQueue(), pSwapchain->as<VulkanSwapchain>());
					}
				, pSwapchain);
				break;

			case Xenon::Backend::CommandRecorderUsage::Transfer:
				m_pDevice->getTransferQueue().access([this](const VulkanQueue& queue) { m_pCurrentBuffer->submit(VK_PIPELINE_STAGE_TRANSFER_BIT, queue.getQueue()); });
				break;

			default:
				XENON_LOG_FATAL("Cannot submit the recorded commands! The command recorder usage used to create the recorder is invalid.");
				break;
			}
		}

		void VulkanCommandRecorder::wait(uint64_t timeout /*= UINT64_MAX*/)
		{
			m_pCurrentBuffer->wait(timeout);
		}
	}
}