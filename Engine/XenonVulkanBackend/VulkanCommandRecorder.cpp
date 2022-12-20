// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandRecorder.hpp"
#include "VulkanMacros.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanRasterizer.hpp"
#include "VulkanRasterizingPipeline.hpp"
#include "VulkanDescriptor.hpp"

#include <optick.h>

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
		OPTICK_EVENT();

		auto itr = clearValues.begin();

		std::vector<VkClearValue> vkClearValues;
		if (attachmentTypes & Xenon::Backend::AttachmentType::Color)
		{
			try
			{
				const auto& clearColor = std::get<glm::vec4>(*(itr++));

				auto& clearValue = vkClearValues.emplace_back();
				clearValue.color.float32[0] = clearColor.x;
				clearValue.color.float32[1] = clearColor.y;
				clearValue.color.float32[2] = clearColor.z;
				clearValue.color.float32[3] = clearColor.w;
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear color value error: {}", e.what());
			}
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::EntityID)
		{
			try
			{
				const auto& clearColor = std::get<glm::vec3>(*(itr++));

				auto& clearValue = vkClearValues.emplace_back();
				clearValue.color.float32[0] = clearColor.x;
				clearValue.color.float32[1] = clearColor.y;
				clearValue.color.float32[2] = clearColor.z;
				clearValue.color.float32[3] = 0.0f;
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear entity ID value error: {}", e.what());
			}
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::Normal)
		{
			try
			{
				const auto clearColor = std::get<float>(*(itr++));

				auto& clearValue = vkClearValues.emplace_back();
				clearValue.color.float32[0] = clearColor;
				clearValue.color.float32[1] = 0.0f;
				clearValue.color.float32[2] = 0.0f;
				clearValue.color.float32[3] = 0.0f;
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear normal value error: {}", e.what());
			}
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::Depth && attachmentTypes & Xenon::Backend::AttachmentType::Stencil)
		{
			try
			{
				auto& clearValue = vkClearValues.emplace_back();
				clearValue.depthStencil.depth = std::get<float>(*(itr++));
				clearValue.depthStencil.stencil = std::get<uint32_t>(*(itr++));
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear depth and stencil value error: {}", e.what());
			}
		}

		else if (attachmentTypes & Xenon::Backend::AttachmentType::Depth)
		{
			try
			{
				vkClearValues.emplace_back().depthStencil.depth = std::get<float>(*(itr++));
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear depth value error: {}", e.what());
			}
		}

		else if (attachmentTypes & Xenon::Backend::AttachmentType::Stencil)
		{
			try
			{
				vkClearValues.emplace_back().depthStencil.stencil = std::get<uint32_t>(*(itr++));
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear stencil value error: {}", e.what());
			}
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
			auto function = [this](const VulkanQueue& queue)
			{
				// Setup the command pool create info structure.
				VkCommandPoolCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

				// Create the compute command pool.
				createInfo.queueFamilyIndex = queue.getFamily();
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateCommandPool(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_CommandPool), "Failed to create the command pool!");
			};

			// Get the command pool from the device and create the buffers.
			VkPipelineStageFlags stageFlags = 0;
			switch (usage)
			{
			case Xenon::Backend::CommandRecorderUsage::Compute:
				pDevice->getComputeQueue().access(std::move(function));
				stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				break;

			case Xenon::Backend::CommandRecorderUsage::Graphics:
				pDevice->getGraphicsQueue().access(std::move(function));
				stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				break;

			case Xenon::Backend::CommandRecorderUsage::Transfer:
				pDevice->getTransferQueue().access(std::move(function));
				stageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;

			case Xenon::Backend::CommandRecorderUsage::Secondary:
				pDevice->getGraphicsQueue().access(std::move(function));
				stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				break;

			default:
				XENON_LOG_FATAL("Invalid command recorder usage!");
			}

			// Allocate the command buffers.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = m_CommandPool;
			allocateInfo.level = m_Usage & CommandRecorderUsage::Secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = bufferCount;

			std::vector<VkCommandBuffer> commandBuffers(bufferCount);
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkAllocateCommandBuffers(m_pDevice->getLogicalDevice(), &allocateInfo, commandBuffers.data()), "Failed to allocate command buffers!");

			// Create the command buffers.
			for (uint32_t i = 0; i < bufferCount; i++)
				m_CommandBuffers.emplace_back(m_pDevice, commandBuffers[i], m_CommandPool, stageFlags);

			// Select the default buffer.
			m_pCurrentBuffer = &m_CommandBuffers[m_CurrentIndex];

			// Setup the initial inheritance info structure data.
			m_InheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			m_InheritanceInfo.pNext = nullptr;
			m_InheritanceInfo.subpass = 0;
			m_InheritanceInfo.occlusionQueryEnable = VK_FALSE;
			m_InheritanceInfo.queryFlags = 0;
			m_InheritanceInfo.pipelineStatistics = 0;
		}

		VulkanCommandRecorder::~VulkanCommandRecorder()
		{
			m_CommandBuffers.clear();
			m_pDevice->getDeviceTable().vkDestroyCommandPool(m_pDevice->getLogicalDevice(), m_CommandPool, nullptr);
		}

		void VulkanCommandRecorder::begin()
		{
			OPTICK_EVENT();

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
			OPTICK_EVENT();

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
			OPTICK_EVENT();

			// Unbind the previous render pass if we need to.
			if (m_IsRenderTargetBound)
			{
				m_IsRenderTargetBound = false;
				m_pDevice->getDeviceTable().vkCmdEndRenderPass(*m_pCurrentBuffer);
			}

			// Create the memory barrier.
			VkImageMemoryBarrier memorybarrier = {};
			memorybarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memorybarrier.srcAccessMask = VK_ACCESS_NONE;
			memorybarrier.dstAccessMask = VK_ACCESS_NONE;
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
				memorybarrier.srcAccessMask = VK_ACCESS_NONE;
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
			OPTICK_EVENT();

			// Unbind a render target if one is already bound.
			if (m_IsRenderTargetBound)
			{
				m_IsRenderTargetBound = false;
				m_pDevice->getDeviceTable().vkCmdEndRenderPass(*m_pCurrentBuffer);
			}

			VkBufferCopy bufferCopy = {};
			bufferCopy.size = size;
			bufferCopy.srcOffset = srcOffset;
			bufferCopy.dstOffset = dstOffset;

			m_pDevice->getDeviceTable().vkCmdCopyBuffer(
				*m_pCurrentBuffer,
				pSource->as<VulkanBuffer>()->getBuffer(),
				pDestination->as<VulkanBuffer>()->getBuffer(),
				1,
				&bufferCopy
			);
		}

		void VulkanCommandRecorder::copy(Image* pSource, Swapchain* pDestination)
		{
			OPTICK_EVENT();

			auto pVkImage = pSource->as<VulkanImage>();
			auto pVkSwapchain = pDestination->as<VulkanSwapchain>();

			// Return if we're minimized.
			if (!pVkSwapchain->isRenderable())
				return;

			const auto currentSwapchainImage = pVkSwapchain->getCurrentImage();

			VkImageBlit blit = {};
			blit.srcSubresource.aspectMask = pVkImage->getAspectFlags();
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
			m_pDevice->getDeviceTable().vkCmdBlitImage(
				*m_pCurrentBuffer,
				pVkImage->getImage(),
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				currentSwapchainImage,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&blit,
				VK_FILTER_LINEAR
			);

			// Change back to previous.
			changeImageLayout(pVkImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pVkImage->getImageLayout(), blit.srcSubresource.aspectMask);
			changeImageLayout(currentSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		void VulkanCommandRecorder::copy(Image* pSource, const glm::vec3& sourceOffset, Image* pDestination, const glm::vec3& destinationOffset)
		{
			OPTICK_EVENT();

			auto pVkSourceImage = pSource->as<VulkanImage>();
			auto pVkDestinationImage = pDestination->as<VulkanImage>();

			VkImageBlit blit = {};
			blit.srcSubresource.aspectMask = pVkSourceImage->getAspectFlags();
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.srcSubresource.mipLevel = 0;
			blit.srcOffsets[0].x = static_cast<int32_t>(sourceOffset.x);
			blit.srcOffsets[0].y = static_cast<int32_t>(sourceOffset.y);
			blit.srcOffsets[0].z = static_cast<int32_t>(sourceOffset.z);
			blit.srcOffsets[1].x = static_cast<int32_t>(pVkSourceImage->getWidth()) - blit.srcOffsets[0].x;
			blit.srcOffsets[1].y = static_cast<int32_t>(pVkSourceImage->getHeight()) - blit.srcOffsets[0].y;
			blit.srcOffsets[1].z = static_cast<int32_t>(pVkSourceImage->getDepth()) - blit.srcOffsets[0].z;
			blit.dstSubresource.aspectMask = pVkDestinationImage->getAspectFlags();
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;
			blit.dstSubresource.mipLevel = 0;
			blit.dstOffsets[0].x = static_cast<int32_t>(destinationOffset.x);
			blit.dstOffsets[0].y = static_cast<int32_t>(destinationOffset.y);
			blit.dstOffsets[0].z = static_cast<int32_t>(destinationOffset.z);
			blit.dstOffsets[1].x = static_cast<int32_t>(pVkDestinationImage->getWidth()) - blit.dstOffsets[0].x;
			blit.dstOffsets[1].y = static_cast<int32_t>(pVkDestinationImage->getHeight()) - blit.dstOffsets[0].y;
			blit.dstOffsets[1].z = static_cast<int32_t>(pVkDestinationImage->getDepth()) - blit.dstOffsets[0].z;

			// Prepare to transfer.
			changeImageLayout(pVkSourceImage->getImage(), pVkSourceImage->getImageLayout(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, blit.srcSubresource.aspectMask);
			changeImageLayout(pVkDestinationImage->getImage(), pVkDestinationImage->getImageLayout(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, blit.dstSubresource.aspectMask);

			// Copy the image.
			m_pDevice->getDeviceTable().vkCmdBlitImage(
				*m_pCurrentBuffer,
				pVkSourceImage->getImage(),
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				pVkDestinationImage->getImage(),
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&blit,
				VK_FILTER_LINEAR
			);

			// Change back to previous.
			if (pVkSourceImage->getImageLayout() == VK_IMAGE_LAYOUT_UNDEFINED)
			{
				auto newLayout = VK_IMAGE_LAYOUT_GENERAL;
				const auto usage = pVkSourceImage->getUsage();
				if (usage & ImageUsage::Graphics)
					newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				else if (usage & ImageUsage::Storage)
					newLayout = VK_IMAGE_LAYOUT_GENERAL;

				else if (usage & ImageUsage::ColorAttachment)
					newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				else if (usage & ImageUsage::DepthAttachment)
					newLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;

				changeImageLayout(pVkSourceImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, newLayout, blit.srcSubresource.aspectMask);
				pVkSourceImage->setImageLayout(newLayout);
			}
			else
			{
				changeImageLayout(pVkSourceImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pVkSourceImage->getImageLayout(), blit.srcSubresource.aspectMask);
			}

			if (pVkDestinationImage->getImageLayout() == VK_IMAGE_LAYOUT_UNDEFINED)
			{
				auto newLayout = VK_IMAGE_LAYOUT_GENERAL;
				const auto usage = pVkDestinationImage->getUsage();
				if (usage & ImageUsage::Graphics)
					newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				else if (usage & ImageUsage::Storage)
					newLayout = VK_IMAGE_LAYOUT_GENERAL;

				else if (usage & ImageUsage::ColorAttachment)
					newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				else if (usage & ImageUsage::DepthAttachment)
					newLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;

				changeImageLayout(pVkDestinationImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, newLayout, blit.dstSubresource.aspectMask);
				pVkDestinationImage->setImageLayout(newLayout);
			}
			else
			{
				changeImageLayout(pVkDestinationImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, pVkDestinationImage->getImageLayout(), blit.dstSubresource.aspectMask);
			}
		}

		void VulkanCommandRecorder::copy(Buffer* pSource, uint64_t bufferOffset, Image* pImage, glm::vec3 imageSize, glm::vec3 imageOffset /*= glm::vec3(0)*/)
		{
			OPTICK_EVENT();

			VkBufferImageCopy imageCopy = {};
			imageCopy.bufferOffset = bufferOffset;
			imageCopy.bufferRowLength = static_cast<uint32_t>(imageSize.x);
			imageCopy.bufferImageHeight = static_cast<uint32_t>(imageSize.y);
			imageCopy.imageSubresource.aspectMask = pImage->getUsage() & ImageUsage::DepthAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopy.imageSubresource.baseArrayLayer = 0;
			imageCopy.imageSubresource.layerCount = 1;
			imageCopy.imageSubresource.mipLevel = 0;
			imageCopy.imageOffset.x = static_cast<int32_t>(imageOffset.x);
			imageCopy.imageOffset.y = static_cast<int32_t>(imageOffset.y);
			imageCopy.imageOffset.z = static_cast<int32_t>(imageOffset.z);
			imageCopy.imageExtent.depth = static_cast<uint32_t>(imageSize.z);
			imageCopy.imageExtent.width = static_cast<uint32_t>(imageSize.x);
			imageCopy.imageExtent.height = static_cast<uint32_t>(imageSize.y);

			m_pDevice->getDeviceTable().vkCmdCopyBufferToImage(
				*m_pCurrentBuffer,
				pSource->as<VulkanBuffer>()->getBuffer(),
				pImage->as<VulkanImage>()->getImage(),
				pImage->as<VulkanImage>()->getImageLayout(),
				1,
				&imageCopy
			);
		}

		void VulkanCommandRecorder::bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues, bool usingSecondaryCommandRecorders /*= false*/)
		{
			OPTICK_EVENT();

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
			OPTICK_EVENT();

			m_pDevice->getDeviceTable().vkCmdBindPipeline(*m_pCurrentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->as<VulkanRasterizingPipeline>()->getPipeline(vertexSpecification).m_Pipeline);
		}

		void VulkanCommandRecorder::bind(RasterizingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pCameraDescriptor)
		{
			OPTICK_EVENT();

			if (pUserDefinedDescriptor)
			{
				auto descriptorSet = pUserDefinedDescriptor->as<VulkanDescriptor>()->getDescriptorSet();
				m_pDevice->getDeviceTable().vkCmdBindDescriptorSets(
					*m_pCurrentBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					pPipeline->as<VulkanRasterizingPipeline>()->getPipelineLayout(),
					0,
					1,
					&descriptorSet,
					0,
					nullptr
				);
			}

			if (pMaterialDescriptor)
			{
				auto descriptorSet = pMaterialDescriptor->as<VulkanDescriptor>()->getDescriptorSet();
				m_pDevice->getDeviceTable().vkCmdBindDescriptorSets(
					*m_pCurrentBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					pPipeline->as<VulkanRasterizingPipeline>()->getPipelineLayout(),
					1,
					1,
					&descriptorSet,
					0,
					nullptr
				);
			}

			if (pCameraDescriptor)
			{
				auto descriptorSet = pCameraDescriptor->as<VulkanDescriptor>()->getDescriptorSet();
				m_pDevice->getDeviceTable().vkCmdBindDescriptorSets(
					*m_pCurrentBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					pPipeline->as<VulkanRasterizingPipeline>()->getPipelineLayout(),
					2,
					1,
					&descriptorSet,
					0,
					nullptr
				);
			}
		}

		void VulkanCommandRecorder::bind(Buffer* pVertexBuffer, uint32_t vertexStride)
		{
			OPTICK_EVENT();

			VkDeviceSize offset = 0;
			VkBuffer vertexBuffer = pVertexBuffer->as<VulkanBuffer>()->getBuffer();
			m_pDevice->getDeviceTable().vkCmdBindVertexBuffers(*m_pCurrentBuffer, 0, 1, &vertexBuffer, &offset);
		}

		void VulkanCommandRecorder::bind(Buffer* pIndexBuffer, IndexBufferStride indexStride)
		{
			OPTICK_EVENT();

			auto indexType = VK_INDEX_TYPE_NONE_KHR;
			if (indexStride == IndexBufferStride::Uint16)
				indexType = VK_INDEX_TYPE_UINT16;

			else if (indexStride == IndexBufferStride::Uint32)
				indexType = VK_INDEX_TYPE_UINT32;

			else
				XENON_LOG_ERROR("Invalid or unsupported index stride!");

			m_pDevice->getDeviceTable().vkCmdBindIndexBuffer(*m_pCurrentBuffer, pIndexBuffer->as<VulkanBuffer>()->getBuffer(), 0, indexType);
		}

		void VulkanCommandRecorder::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
		{
			VkViewport viewport = {};
			viewport.x = x;
			viewport.y = y;
			viewport.width = width;
			viewport.height = height;
			viewport.minDepth = minDepth;
			viewport.maxDepth = maxDepth;
			m_pDevice->getDeviceTable().vkCmdSetViewport(*m_pCurrentBuffer, 0, 1, &viewport);
		}

		void VulkanCommandRecorder::setViewportNatural(float x, float y, float width, float height, float minDepth, float maxDepth)
		{
			setViewport(x, y, width, height, minDepth, maxDepth);
		}

		void VulkanCommandRecorder::setScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
		{
			VkRect2D scissorRect = {};
			scissorRect.offset.x = x;
			scissorRect.offset.y = y;
			scissorRect.extent.width = width;
			scissorRect.extent.height = height;
			m_pDevice->getDeviceTable().vkCmdSetScissor(*m_pCurrentBuffer, 0, 1, &scissorRect);
		}

		void VulkanCommandRecorder::drawIndexed(uint64_t vertexOffset, uint64_t indexOffset, uint64_t indexCount, uint32_t instanceCount /*= 1*/, uint32_t firstInstance /*= 0*/)
		{
			OPTICK_EVENT();

			// m_pDevice->getDeviceTable().vkCmdSetPrimitiveTopology(*m_pCurrentBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
			m_pDevice->getDeviceTable().vkCmdDrawIndexed(*m_pCurrentBuffer, static_cast<uint32_t>(indexCount), instanceCount, static_cast<uint32_t>(indexOffset), static_cast<uint32_t>(vertexOffset), firstInstance);
		}

		void VulkanCommandRecorder::executeChildren()
		{
			OPTICK_EVENT();

			// Skip if we don't have any children :(
			if (m_ChildCommandBuffers.empty())
				return;

			m_pDevice->getDeviceTable().vkCmdExecuteCommands(*m_pCurrentBuffer, static_cast<uint32_t>(m_ChildCommandBuffers.size()), m_ChildCommandBuffers.data());
			m_ChildCommandBuffers.clear();
		}

		void VulkanCommandRecorder::end()
		{
			OPTICK_EVENT();

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
			OPTICK_EVENT();

			m_pCurrentBuffer = &m_CommandBuffers[incrementIndex()];
		}

		void VulkanCommandRecorder::submit(Swapchain* pSwapchain /*= nullptr*/)
		{
			OPTICK_EVENT();

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
			OPTICK_EVENT();

			m_pCurrentBuffer->wait(timeout);
		}
	}
}