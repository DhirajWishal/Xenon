// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Factory.hpp"

#include "DX12Instance.hpp"
#include "DX12Device.hpp"
#include "DX12CommandRecorder.hpp"
#include "DX12Buffer.hpp"
#include "DX12Image.hpp"
#include "DX12Rasterizer.hpp"
#include "DX12Swapchain.hpp"

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
			return std::make_unique<Xenon::Backend::DX12Rasterizer>(pDevice->as<DX12Device>(), pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount);
		}

		std::unique_ptr<Xenon::Backend::Swapchain> DX12Factory::createSwapchain(Device* pDevice, const std::string& title, uint32_t width, uint32_t height)
		{
			return std::make_unique<Xenon::Backend::DX12Swapchain>(pDevice->as<DX12Device>(), title, width, height);
		}
	}
}