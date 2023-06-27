// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Descriptor.hpp"
#include "DX12Macros.hpp"
#include "DX12Buffer.hpp"
#include "DX12ImageView.hpp"
#include "DX12ImageSampler.hpp"

#include <optick.h>

namespace /* anonymous */
{
	/**
	 * Get the descriptor range type.
	 *
	 * @param resource The Xenon resource type.
	 * @return The D3D12 descriptor range type.
	 */
	XENON_NODISCARD constexpr D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangeType(Xenon::Backend::ResourceType resource) noexcept
	{
		switch (resource)
		{
		case Xenon::Backend::ResourceType::Sampler:
		case Xenon::Backend::ResourceType::CombinedImageSampler:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;

		case Xenon::Backend::ResourceType::SampledImage:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::StorageImage:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::UniformTexelBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::StorageTexelBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::UniformBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::StorageBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::DynamicUniformBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::DynamicStorageBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::InputAttachment:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::AccelerationStructure:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		default:
			XENON_LOG_ERROR("Invalid resource type! Defaulting to SRV.");
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12Descriptor::DX12Descriptor(DX12Device* pDevice, const std::unordered_map<uint32_t, DescriptorBindingInfo>& bindingInfo, DescriptorType type, const std::unordered_map<uint32_t, UINT>& bindingOffsets, DX12DescriptorHeapManager* pManager)
			: Descriptor(pDevice, bindingInfo, type)
			, DX12DeviceBoundObject(pDevice)
			, m_BindingOffsets(bindingOffsets)
			, m_pManager(pManager)
		{
			const auto [buffers, samplers] = m_pManager->setupDescriptor(type);
			m_CbvSrvUavDescriptorHeapStart = buffers;
			m_SamplerDescriptorHeapStart = samplers;
		}

		DX12Descriptor::~DX12Descriptor()
		{
			m_pManager->freeDescriptor(m_CbvSrvUavDescriptorHeapStart, m_SamplerDescriptorHeapStart);
		}

		void DX12Descriptor::attach(uint32_t binding, Buffer* pBuffer)
		{
			OPTICK_EVENT();

			// Return if we don't have the binding. Might be because of shader optimizations.
			if (!m_BindingInformation.contains(binding))
				return;

			auto pDx12Buffer = pBuffer->as<DX12Buffer>();
			const auto type = m_BindingInformation[binding].m_Type;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pManager->getCbvSrvUavHeapStartCPU(), m_CbvSrvUavDescriptorHeapStart + m_BindingOffsets[binding], m_pManager->getCbvSrvUavHeapIncrementSize());

			if (type == ResourceType::UniformBuffer || type == ResourceType::DynamicUniformBuffer)
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = pDx12Buffer->getResource()->GetGPUVirtualAddress();
				desc.SizeInBytes = static_cast<UINT>(pDx12Buffer->getSize());

				m_pDevice->getDevice()->CreateConstantBufferView(&desc, handle);
			}
			else if (type == ResourceType::StorageBuffer || type == ResourceType::DynamicStorageBuffer)
			{
				D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
				desc.Buffer.FirstElement = 0;
				desc.Buffer.NumElements = 1;
				desc.Buffer.StructureByteStride = static_cast<UINT>(pDx12Buffer->getSize());
				desc.Buffer.CounterOffsetInBytes = 0;
				desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

				m_pDevice->getDevice()->CreateUnorderedAccessView(pDx12Buffer->getResource(), nullptr, &desc, handle);
			}
			else
			{
				XENON_LOG_ERROR("Invalid buffer resource type!");
			}

			m_pManager->notifyHeapUpdated();
		}

		void DX12Descriptor::attach(uint32_t binding, Image* pImage, ImageView* pView, ImageSampler* pSampler, ImageUsage usage)
		{
			OPTICK_EVENT();

			// Return if we don't have the binding. Might be because of shader optimizations.
			if (!m_BindingInformation.contains(binding))
				return;

			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_pManager->getCbvSrvUavHeapStartCPU(), m_CbvSrvUavDescriptorHeapStart + m_BindingOffsets[binding], m_pManager->getCbvSrvUavHeapIncrementSize());

			if (usage & ImageUsage::Graphics || usage & ImageUsage::ColorAttachment || usage & ImageUsage::DepthAttachment)
			{
				m_pDevice->getDevice()->CreateShaderResourceView(pImage->as<DX12Image>()->getResource(), pView->as<DX12ImageView>()->getSRVDescriptionPtr(), handle);

				auto samplerHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pManager->getSamplerHeapStartCPU(), m_SamplerDescriptorHeapStart + m_pManager->getSamplerIndex(binding), m_pManager->getSamplerHeapIncrementSize());
				m_pDevice->getDevice()->CreateSampler(pSampler->as<DX12ImageSampler>()->getSamplerDescriptionPtr(), samplerHandle);
			}
			else if (usage & ImageUsage::Storage)
			{
				m_pDevice->getDevice()->CreateUnorderedAccessView(pImage->as<DX12Image>()->getResource(), nullptr, pView->as<DX12ImageView>()->getUAVDescriptionPtr(), handle);
			}
			else
			{
				XENON_LOG_ERROR("Invalid image usage!");
			}

			m_pManager->notifyHeapUpdated();
		}
	}
}