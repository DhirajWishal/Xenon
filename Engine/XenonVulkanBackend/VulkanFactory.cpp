// Copyright 2022-2023 Dhiraj Wishal
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
#include "VulkanComputePipeline.hpp"
#include "VulkanCommandSubmitter.hpp"
#include "VulkanOcclusionQuery.hpp"
#include "VulkanRayTracer.hpp"
#include "VulkanBottomLevelAccelerationStructure.hpp"
#include "VulkanTopLevelAccelerationStructure.hpp"
#include "VulkanRayTracingPipeline.hpp"

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

		std::unique_ptr<Xenon::Backend::Rasterizer> VulkanFactory::createRasterizer(Device* pDevice, uint32_t width, uint32_t height, AttachmentType attachmentTypes, bool enableTripleBuffering /*= false*/, MultiSamplingCount multiSampleCount /*= MultiSamplingCount::x1*/)
		{
			return std::make_unique<VulkanRasterizer>(pDevice->as<VulkanDevice>(), width, height, attachmentTypes, enableTripleBuffering, multiSampleCount);
		}

		std::unique_ptr<Xenon::Backend::Swapchain> VulkanFactory::createSwapchain(Device* pDevice, const std::string& title, uint32_t width, uint32_t height)
		{
			return std::make_unique<VulkanSwapchain>(pDevice->as<VulkanDevice>(), title, width, height);
		}

		std::unique_ptr<Xenon::Backend::ImageView> VulkanFactory::createImageView(Device* pDevice, Image* pImage, const ImageViewSpecification& specification)
		{
			return std::make_unique<VulkanImageView>(pDevice->as<VulkanDevice>(), pImage->as<VulkanImage>(), specification);
		}

		std::unique_ptr<Xenon::Backend::ImageSampler> VulkanFactory::createImageSampler(Device* pDevice, const ImageSamplerSpecification& specification)
		{
			return std::make_unique<VulkanImageSampler>(pDevice->as<VulkanDevice>(), specification);
		}

		std::unique_ptr<Xenon::Backend::RasterizingPipeline> VulkanFactory::createRasterizingPipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, Rasterizer* pRasterizer, const RasterizingPipelineSpecification& specification)
		{
			return std::make_unique<VulkanRasterizingPipeline>(pDevice->as<VulkanDevice>(), std::move(pCacheHandler), pRasterizer->as<VulkanRasterizer>(), specification);
		}

		std::unique_ptr<Xenon::Backend::ComputePipeline> VulkanFactory::createComputePipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const Shader& computeShader)
		{
			return std::make_unique<VulkanComputePipeline>(pDevice->as<VulkanDevice>(), std::move(pCacheHandler), computeShader);
		}

		std::unique_ptr<Xenon::Backend::CommandSubmitter> VulkanFactory::createCommandSubmitter(Device* pDevice)
		{
			return std::make_unique<VulkanCommandSubmitter>(pDevice->as<VulkanDevice>());
		}

		std::unique_ptr<Xenon::Backend::OcclusionQuery> VulkanFactory::createOcclusionQuery(Device* pDevice, uint64_t sampleCount)
		{
			return std::make_unique<VulkanOcclusionQuery>(pDevice->as<VulkanDevice>(), sampleCount);
		}

		std::unique_ptr<Xenon::Backend::TopLevelAccelerationStructure> VulkanFactory::createTopLevelAccelerationStructure(Device* pDevice, const std::vector<BottomLevelAccelerationStructure*>& pBottomLevelAccelerationStructures)
		{
			return std::make_unique<VulkanTopLevelAccelerationStructure>(pDevice->as<VulkanDevice>(), pBottomLevelAccelerationStructures);
		}

		std::unique_ptr<Xenon::Backend::BottomLevelAccelerationStructure> VulkanFactory::createBottomLevelAccelerationStructure(Device* pDevice, const std::vector<AccelerationStructureGeometry>& geometries)
		{
			return std::make_unique<VulkanBottomLevelAccelerationStructure>(pDevice->as<VulkanDevice>(), geometries);
		}

		std::unique_ptr<Xenon::Backend::RayTracer> VulkanFactory::createRayTracer(Device* pDevice, uint32_t width, uint32_t height)
		{
			return std::make_unique<VulkanRayTracer>(pDevice->as<VulkanDevice>(), width, height);
		}

		std::unique_ptr<Xenon::Backend::RayTracingPipeline> VulkanFactory::createRayTracingPipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const RayTracingPipelineSpecification& specification)
		{
			return std::make_unique<VulkanRayTracingPipeline>(pDevice->as<VulkanDevice>(), std::move(pCacheHandler), specification);
		}
	}
}