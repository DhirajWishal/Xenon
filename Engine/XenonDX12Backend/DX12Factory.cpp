// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Factory.hpp"

#include "DX12Instance.hpp"
#include "DX12Device.hpp"
#include "DX12VertexBuffer.hpp"
#include "DX12IndexBuffer.hpp"
#include "DX12StagingBuffer.hpp"

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

		std::unique_ptr<Xenon::Backend::IndexBuffer> DX12Factory::createIndexBuffer(Device* pDevice, uint64_t size, IndexSize indexSize)
		{
			return std::make_unique<DX12IndexBuffer>(pDevice->as<DX12Device>(), size, indexSize);
		}

		std::unique_ptr<Xenon::Backend::VertexBuffer> DX12Factory::createVertexBuffer(Device* pDevice, uint64_t size, uint64_t stride)
		{
			return std::make_unique<DX12VertexBuffer>(pDevice->as<DX12Device>(), size, stride);
		}

		std::unique_ptr<Xenon::Backend::StagingBuffer> DX12Factory::createStagingBuffer(Device* pDevice, uint64_t size)
		{
			return std::make_unique<DX12StagingBuffer>(pDevice->as<DX12Device>(), size);
		}

		std::unique_ptr<Xenon::Backend::StorageBuffer> DX12Factory::createStorageBuffer(Device* pDevice, uint64_t size)
		{
			return nullptr;
		}

		std::unique_ptr<Xenon::Backend::UniformBuffer> DX12Factory::createUniformBuffer(Device* pDevice, uint64_t size)
		{
			return nullptr;
		}
	}
}