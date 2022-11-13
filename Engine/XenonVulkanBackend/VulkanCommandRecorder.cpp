// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandRecorder.hpp"
#include "VulkanMacros.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanRasterizer.hpp"

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
			const auto clearColor = std::get<glm::vec4>(*itr);

			VkClearValue& clearValue = vkClearValues.emplace_back();
			clearValue.color.float32[0] = clearColor.x;
			clearValue.color.float32[1] = clearColor.y;
			clearValue.color.float32[2] = clearColor.z;
			clearValue.color.float32[3] = clearColor.w;

			++itr;
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::EntityID)
		{
			const auto clearColor = std::get<glm::vec3>(*itr);

			VkClearValue& clearValue = vkClearValues.emplace_back();
			clearValue.color.float32[0] = clearColor.x;
			clearValue.color.float32[1] = clearColor.y;
			clearValue.color.float32[2] = clearColor.z;
			clearValue.color.float32[3] = 0.0f;

			++itr;
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::Normal)
		{
			const auto clearColor = std::get<float>(*itr);

			VkClearValue& clearValue = vkClearValues.emplace_back();
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
}

namespace Xenon
{
	namespace Backend
	{
		VulkanCommandRecorder::VulkanCommandRecorder(VulkanDevice* pDevice, CommandRecorderUsage usage, uint32_t bufferCount /*= 1*/)
			: VulkanDeviceBoundObject(pDevice)
			, CommandRecorder(pDevice, usage, bufferCount)
		{
			// Get the command pool from the device.
			VkCommandPool commandPool = VK_NULL_HANDLE;
			switch (usage)
			{
			case Xenon::Backend::CommandRecorderUsage::Compute:
				commandPool = pDevice->getComputeCommandPool();
				break;

			case Xenon::Backend::CommandRecorderUsage::Graphics:
				commandPool = pDevice->getGraphicsCommandPool();
				break;

			case Xenon::Backend::CommandRecorderUsage::Transfer:
				commandPool = pDevice->getTransferCommandPool();
				break;

			default:
				XENON_LOG_FATAL("Invalid command recorder usage!");
				return;
			}

			// Allocate the command buffers.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = commandPool;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = bufferCount;

			std::vector<VkCommandBuffer> commandBuffers(bufferCount);
			XENON_VK_ASSERT(pDevice->getDeviceTable().vkAllocateCommandBuffers(pDevice->getLogicalDevice(), &allocateInfo, commandBuffers.data()), "Failed to allocate command buffers!");

			// Create the command buffers.
			for (uint32_t i = 0; i < bufferCount; i++)
				m_CommandBuffers.emplace_back(pDevice, commandBuffers[i], commandPool);

			// Select the default buffer.
			m_pCurrentBuffer = &m_CommandBuffers[m_CurrentIndex];
		}

		void VulkanCommandRecorder::begin()
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = VK_NULL_HANDLE;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			m_pDevice->getDeviceTable().vkBeginCommandBuffer(*m_pCurrentBuffer, &beginInfo);
		}

		void VulkanCommandRecorder::copy(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size)
		{
			VkBufferCopy bufferCopy = {};
			bufferCopy.size = size;
			bufferCopy.srcOffset = srcOffset;
			bufferCopy.dstOffset = dstOffset;

			m_pDevice->getDeviceTable().vkCmdCopyBuffer(*m_pCurrentBuffer, pSource->as<VulkanBuffer>()->getBuffer(), pDestination->as<VulkanBuffer>()->getBuffer(), 1, &bufferCopy);
		}

		void VulkanCommandRecorder::bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues)
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

			m_pDevice->getDeviceTable().vkCmdBeginRenderPass(*m_pCurrentBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
			m_IsRenderTargetBound = true;
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

		void VulkanCommandRecorder::submit(Swapchain* pSawpchain /*= nullptr*/)
		{
			switch (m_Usage)
			{
			case Xenon::Backend::CommandRecorderUsage::Compute:
				m_pCurrentBuffer->submit(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, m_pDevice->getComputeQueue().getQueue());
				break;

			case Xenon::Backend::CommandRecorderUsage::Graphics:
				m_pCurrentBuffer->submit(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_pDevice->getGraphicsQueue().getQueue(), pSawpchain->as<VulkanSwapchain>());
				break;

			case Xenon::Backend::CommandRecorderUsage::Transfer:
				m_pCurrentBuffer->submit(VK_PIPELINE_STAGE_TRANSFER_BIT, m_pDevice->getTransferQueue().getQueue());
				break;

			default:
				XENON_LOG_FATAL("Cannot submit the recorded commands! The command recorder usage used to create the recorder is invalid.");
				break;
			}
		}

		void VulkanCommandRecorder::wait(uint64_t timeout /*= std::numeric_limits<uint64_t>::max()*/)
		{
			m_pCurrentBuffer->wait(timeout);
		}
	}
}