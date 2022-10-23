// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Factory.hpp"

#include "DX12Instance.hpp"
#include "DX12Device.hpp"
#include "DX12CommandRecorder.hpp"
#include "DX12Buffer.hpp"

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
	}
}