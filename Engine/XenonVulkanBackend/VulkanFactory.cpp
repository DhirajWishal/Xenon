// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanFactory.hpp"
#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanCommandRecorder.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanRasterizer.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanDescriptor.hpp"
#include "VulkanImageView.hpp"
#include "VulkanImageSampler.hpp"
#include "VulkanRasterizingPipeline.hpp"

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

		std::unique_ptr<Xenon::Backend::CommandRecorder> VulkanFactory::createCommandRecorder(Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount /*= 1*/)
		{
			return std::make_unique<VulkanCommandRecorder>(pDevice->as<VulkanDevice>(), usage, bufferCount);
		}

		std::unique_ptr<Xenon::Backend::Buffer> VulkanFactory::createBuffer(Device* pDevice, uint64_t size, BufferType type)
		{
			return std::make_unique<VulkanBuffer>(pDevice->as<VulkanDevice>(), size, type);
		}

		std::unique_ptr<Xenon::Backend::Image> VulkanFactory::createImage(Device* pDevice, const ImageSpecification& specification)
		{
			return std::make_unique<VulkanImage>(pDevice->as<VulkanDevice>(), specification);
		}

		std::unique_ptr<Xenon::Backend::Rasterizer> VulkanFactory::createRasterizer(Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering /*= false*/, MultiSamplingCount multiSampleCount /*= MultiSamplingCount::x1*/)
		{
			return std::make_unique<Xenon::Backend::VulkanRasterizer>(pDevice->as<VulkanDevice>(), pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount);
		}

		std::unique_ptr<Xenon::Backend::Swapchain> VulkanFactory::createSwapchain(Device* pDevice, const std::string& title, uint32_t width, uint32_t height)
		{
			return std::make_unique<Xenon::Backend::VulkanSwapchain>(pDevice->as<VulkanDevice>(), title, width, height);
		}

		std::unique_ptr<Xenon::Backend::ImageView> VulkanFactory::createImageView(Device* pDevice, Image* pImage, const ImageViewSpecification& specification)
		{
			return std::make_unique<Xenon::Backend::VulkanImageView>(pDevice->as<VulkanDevice>(), pImage->as<VulkanImage>(), specification);
		}

		std::unique_ptr<Xenon::Backend::ImageSampler> VulkanFactory::createImageSampler(Device* pDevice, const ImageSamplerSpecification& specification)
		{
			return std::make_unique<Xenon::Backend::VulkanImageSampler>(pDevice->as<VulkanDevice>(), specification);
		}

		std::unique_ptr<Xenon::Backend::RasterizingPipeline> VulkanFactory::createRasterizingPipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, Rasterizer* pRasterizer, const RasterizingPipelineSpecification& specification)
		{
			return std::make_unique<VulkanRasterizingPipeline>(pDevice->as<VulkanDevice>(), std::move(pCacheHandler), pRasterizer->as<VulkanRasterizer>(), specification);
		}
	}
}