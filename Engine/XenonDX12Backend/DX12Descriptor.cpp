// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Descriptor.hpp"
#include "DX12Macros.hpp"

#include "DX12Buffer.hpp"
#include "DX12ImageView.hpp"
#include "DX12ImageSampler.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Descriptor::DX12Descriptor(DX12Device* pDevice, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType type)
			: Descriptor(pDevice, bindingInfo, type)
			, DX12DeviceBoundObject(pDevice)
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.NumDescriptors = XENON_DX12_MAX_DESCRIPTOR_COUNT;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			switch (type)
			{
			case Xenon::Backend::DescriptorType::UserDefined:
			case Xenon::Backend::DescriptorType::Material:
				heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
				XENON_DX12_ASSERT(pDevice->getDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_SamplerDescriptorHeap)), "Failed to create the sampler descriptor heap!");

				m_SamplerDescriptorHeapSize = pDevice->getDevice()->GetDescriptorHandleIncrementSize(heapDesc.Type);

			case Xenon::Backend::DescriptorType::Camera:
				heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				XENON_DX12_ASSERT(pDevice->getDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_CbvSrvUavDescriptorHeap)), "Failed to create the buffer descriptor heap!");

				m_CbvSrvUavDescriptorHeapSize = pDevice->getDevice()->GetDescriptorHandleIncrementSize(heapDesc.Type);
				break;

			default:
				XENON_LOG_ERROR("Invalid descriptor type!");
			}
		}

		void DX12Descriptor::attach(uint32_t binding, Buffer* pBuffer)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
			desc.BufferLocation = pBuffer->as<DX12Buffer>()->getResource()->GetGPUVirtualAddress();
			desc.SizeInBytes = static_cast<UINT>(pBuffer->getSize());

			m_pDevice->getDevice()->CreateConstantBufferView(&desc, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), binding, m_CbvSrvUavDescriptorHeapSize));
		}

		void DX12Descriptor::attach(uint32_t binding, Image* pImage, ImageView* pView, ImageSampler* pSampler, ImageUsage usage)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), binding, m_CbvSrvUavDescriptorHeapSize);

			if (usage & ImageUsage::Graphics || usage & ImageUsage::ColorAttachment || usage & ImageUsage::DepthAttachment)
			{
				m_pDevice->getDevice()->CreateShaderResourceView(pImage->as<DX12Image>()->getResource(), pView->as<DX12ImageView>()->getSRVDescriptionPtr(), handle);
				m_pDevice->getDevice()->CreateSampler(pSampler->as<DX12ImageSampler>()->getSamplerDescriptionPtr(), CD3DX12_CPU_DESCRIPTOR_HANDLE(m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), binding, m_SamplerDescriptorHeapSize));
			}
			else if (usage & ImageUsage::Storage)
			{
				m_pDevice->getDevice()->CreateUnorderedAccessView(pImage->as<DX12Image>()->getResource(), nullptr, pView->as<DX12ImageView>()->getUAVDescriptionPtr(), handle);
			}
		}
	}
}