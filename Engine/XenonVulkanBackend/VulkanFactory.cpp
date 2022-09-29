// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanFactory.hpp"

#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanCommandBuffers.hpp"
#include "VulkanIndexBuffer.hpp"
#include "VulkanStagingBuffer.hpp"
#include "VulkanStorageBuffer.hpp"
#include "VulkanUniformBuffer.hpp"
#include "VulkanVertexBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		std::unique_ptr<Xenon::Backend::Instance> VulkanFactory::createInstance(const std::string& applicationName, uint32_t applicationVersion)
		{
			return std::make_unique<VulkanInstance>(applicationName, applicationVersion);
		}

		std::unique_ptr<Xenon::Backend::Device> VulkanFactory::createDevice(Instance* pInstance, RenderTargetType requiredRenderTargets)
		{
			return std::make_unique<VulkanDevice>(pInstance->as<VulkanInstance>(), requiredRenderTargets);
		}

		std::unique_ptr<Xenon::Backend::CommandBuffers> VulkanFactory::createCommandBuffers(Device* pDevice, uint32_t bufferCount)
		{
			return std::make_unique<VulkanCommandBuffers>(pDevice->as<VulkanDevice>(), bufferCount);
		}

		std::unique_ptr<Xenon::Backend::IndexBuffer> VulkanFactory::createIndexBuffer(Device* pDevice, uint64_t size, IndexSize indexSize)
		{
			return std::make_unique<VulkanIndexBuffer>(pDevice->as<VulkanDevice>(), size, indexSize);
		}

		std::unique_ptr<Xenon::Backend::VertexBuffer> VulkanFactory::createVertexBuffer(Device* pDevice, uint64_t size, uint64_t stride)
		{
			return std::make_unique<VulkanVertexBuffer>(pDevice->as<VulkanDevice>(), size, stride);
		}

		std::unique_ptr<Xenon::Backend::StagingBuffer> VulkanFactory::createStagingBuffer(Device* pDevice, uint64_t size)
		{
			return std::make_unique<VulkanStagingBuffer>(pDevice->as<VulkanDevice>(), size);
		}

		std::unique_ptr<Xenon::Backend::StorageBuffer> VulkanFactory::createStorageBuffer(Device* pDevice, uint64_t size)
		{
			return std::make_unique<VulkanStorageBuffer>(pDevice->as<VulkanDevice>(), size);
		}

		std::unique_ptr<Xenon::Backend::UniformBuffer> VulkanFactory::createUniformBuffer(Device* pDevice, uint64_t size)
		{
			return std::make_unique<VulkanUniformBuffer>(pDevice->as<VulkanDevice>(), size);
		}
	}
}