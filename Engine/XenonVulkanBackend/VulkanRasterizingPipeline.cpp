// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRasterizingPipeline.hpp"
#include "VulkanRasterizer.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanRasterizingPipeline::VulkanRasterizingPipeline(VulkanDevice* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, VulkanRasterizer* pRasterizer, const RasterizingPipelineSpecification& specification)
			: RasterizingPipeline(pDevice, std::move(pCacheHandler), pRasterizer, specification)
			, VulkanDeviceBoundObject(pDevice)
		{

		}

		VulkanRasterizingPipeline::~VulkanRasterizingPipeline()
		{

		}
	}
}