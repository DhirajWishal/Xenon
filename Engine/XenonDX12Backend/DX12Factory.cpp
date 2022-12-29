// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Factory.hpp"
#include "DX12Instance.hpp"
#include "DX12Device.hpp"
#include "DX12CommandRecorder.hpp"
#include "DX12Buffer.hpp"
#include "DX12ImageView.hpp"
#include "DX12Rasterizer.hpp"
#include "DX12Swapchain.hpp"
#include "DX12ImageSampler.hpp"
#include "DX12RasterizingPipeline.hpp"
#include "DX12ComputePipeline.hpp"
#include "DX12CommandSubmitter.hpp"
#include "DX12OcclusionQuery.hpp"
#include "DX12RayTracer.hpp"
#include "DX12BottomLevelAccelerationStructure.hpp"
#include "DX12TopLevelAccelerationStructure.hpp"

namespace Xenon
{
	namespace Backend
	{
		std::unique_ptr<Xenon::Backend::Instance> DX12Factory::createInstance(const std::string& applicationName, uint32_t applicationVersion)
		{
			return std::make_unique<DX12Instance>(applicationName, applicationVersion);
		}

		std::unique_ptr<Xenon::Backend::Device> DX12Factory::createDevice(Instance* pInstance, RenderTargetType requiredRenderTargets)
		{
			return std::make_unique<DX12Device>(pInstance->as<DX12Instance>(), requiredRenderTargets);
		}

		std::unique_ptr<Xenon::Backend::CommandRecorder> DX12Factory::createCommandRecorder(Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount /*= 1*/)
		{
			return std::make_unique<DX12CommandRecorder>(pDevice->as<DX12Device>(), usage, bufferCount);
		}

		std::unique_ptr<Xenon::Backend::Buffer> DX12Factory::createBuffer(Device* pDevice, uint64_t size, BufferType type)
		{
			return std::make_unique<DX12Buffer>(pDevice->as<DX12Device>(), size, type);
		}

		std::unique_ptr<Xenon::Backend::Image> DX12Factory::createImage(Device* pDevice, const ImageSpecification& specification)
		{
			return std::make_unique<DX12Image>(pDevice->as<DX12Device>(), specification);
		}

		std::unique_ptr<Xenon::Backend::Rasterizer> DX12Factory::createRasterizer(Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering /*= false*/, MultiSamplingCount multiSampleCount /*= MultiSamplingCount::x1*/)
		{
			return std::make_unique<DX12Rasterizer>(pDevice->as<DX12Device>(), pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount);
		}

		std::unique_ptr<Xenon::Backend::Swapchain> DX12Factory::createSwapchain(Device* pDevice, const std::string& title, uint32_t width, uint32_t height)
		{
			return std::make_unique<DX12Swapchain>(pDevice->as<DX12Device>(), title, width, height);
		}

		std::unique_ptr<Xenon::Backend::ImageView> DX12Factory::createImageView(Device* pDevice, Image* pImage, const ImageViewSpecification& specification)
		{
			return std::make_unique<DX12ImageView>(pDevice->as<DX12Device>(), pImage->as<DX12Image>(), specification);
		}

		std::unique_ptr<Xenon::Backend::ImageSampler> DX12Factory::createImageSampler(Device* pDevice, const ImageSamplerSpecification& specification)
		{
			return std::make_unique<DX12ImageSampler>(pDevice->as<DX12Device>(), specification);
		}

		std::unique_ptr<Xenon::Backend::RasterizingPipeline> DX12Factory::createRasterizingPipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, Rasterizer* pRasterizer, const RasterizingPipelineSpecification& specification)
		{
			return std::make_unique<DX12RasterizingPipeline>(pDevice->as<DX12Device>(), std::move(pCacheHandler), pRasterizer->as<DX12Rasterizer>(), specification);
		}

		std::unique_ptr<Xenon::Backend::ComputePipeline> DX12Factory::createComputePipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const ShaderSource& computeShader)
		{
			return std::make_unique<DX12ComputePipeline>(pDevice->as<DX12Device>(), std::move(pCacheHandler), computeShader);
		}

		std::unique_ptr<Xenon::Backend::CommandSubmitter> DX12Factory::createCommandSubmitter(Device* pDevice)
		{
			return std::make_unique<DX12CommandSubmitter>(pDevice->as<DX12Device>());
		}

		std::unique_ptr<Xenon::Backend::OcclusionQuery> DX12Factory::createOcclusionQuery(Device* pDevice, uint64_t sampleCount)
		{
			return std::make_unique<DX12OcclusionQuery>(pDevice->as<DX12Device>(), sampleCount);
		}

		std::unique_ptr<Xenon::Backend::TopLevelAccelerationStructure> DX12Factory::createTopLevelAccelerationStructure(Device* pDevice, const std::vector<BottomLevelAccelerationStructure*>& pBottomLevelAccelerationStructures)
		{
			return std::make_unique<DX12TopLevelAccelerationStructure>(pDevice->as<DX12Device>(), pBottomLevelAccelerationStructures);
		}

		std::unique_ptr<Xenon::Backend::BottomLevelAccelerationStructure> DX12Factory::createBottomLevelAccelerationStructure(Device* pDevice, const std::vector<AccelerationStructureGeometry>& geometries)
		{
			return std::make_unique<DX12BottomLevelAccelerationStructure>(pDevice->as<DX12Device>(), geometries);
		}

		std::unique_ptr<Xenon::Backend::RayTracer> DX12Factory::createRayTracer(Device* pDevice, Camera* pCamera)
		{
			return std::make_unique<DX12RayTracer>(pDevice->as<DX12Device>(), pCamera);
		}

		std::unique_ptr<Xenon::Backend::RayTracingPipeline> DX12Factory::createRayTracingPipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const std::vector<ShaderGroup>& shaderGroups, uint32_t maxRayRecursion /*= 4*/)
		{
			return nullptr;
		}
	}
}