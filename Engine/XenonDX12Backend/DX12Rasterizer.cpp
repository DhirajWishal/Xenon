// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Rasterizer.hpp"
#include "DX12Macros.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Backend
	{
		DX12Rasterizer::DX12Rasterizer(DX12Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering /*= false*/, MultiSamplingCount multiSampleCount /*= MultiSamplingCount::x1*/)
			: Rasterizer(pDevice, pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount)
			, DX12DeviceBoundObject(pDevice)
		{
			// Setup the descriptor counts.
			const auto frameCount = enableTripleBuffering ? 3 : 1;

			uint8_t colorAttachmentCount = 0;
			if (attachmentTypes & AttachmentType::Color) colorAttachmentCount++;
			if (attachmentTypes & AttachmentType::EntityID) colorAttachmentCount++;
			if (attachmentTypes & AttachmentType::Normal) colorAttachmentCount++;
			const auto colorDescriptorCount = colorAttachmentCount * frameCount;

			uint8_t depthAttachmentCount = 0;
			if (attachmentTypes & AttachmentType::Depth && attachmentTypes & AttachmentType::Stencil) depthAttachmentCount++;
			else if (attachmentTypes & AttachmentType::Depth) depthAttachmentCount++;
			else if (attachmentTypes & AttachmentType::Stencil) depthAttachmentCount++;
			const auto depthDescriptorCount = depthAttachmentCount * frameCount;

			// Create the main heaps.
			if (colorDescriptorCount > 0)
			{
				D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
				rtvHeapDesc.NumDescriptors = colorDescriptorCount;
				rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_ColorTargetHeap)), "Failed to create the Render Target View heap!");
				m_ColorHeapSize = m_pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				XENON_DX12_NAME_OBJECT(m_ColorTargetHeap, "Rasterizer Color Target Heap");

				D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
				srvHeapDesc.NumDescriptors = 1;
				srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_ColorShaderViewHeap)), "Failed to create the Render Target Shader View heap!");
				XENON_DX12_NAME_OBJECT(m_ColorShaderViewHeap, "Rasterizer Shader Visible Color Target Heap");
			}

			if (depthDescriptorCount > 0)
			{
				D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
				dsvHeapDesc.NumDescriptors = depthDescriptorCount;
				dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DepthTargetHeap)));
				m_DepthHeapSize = m_pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
				XENON_DX12_NAME_OBJECT(m_DepthTargetHeap, "Rasterizer Depth Target Heap");

				D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
				srvHeapDesc.NumDescriptors = 1;
				srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_DepthShaderViewHeap)), "Failed to create the Depth Target Shader View heap!");
				XENON_DX12_NAME_OBJECT(m_DepthShaderViewHeap, "Rasterizer Shader Visible Depth Target Heap");
			}

			// Setup the render targets.
			setupRenderTargets();

			// Bind the images to the heaps.
			for (UINT i = 0; i < frameCount; i++)
			{
				auto itr = m_RenderTargets.begin();
				const auto colorIndex = i * colorAttachmentCount;

				// Bind the color attachments.
				if (attachmentTypes & AttachmentType::Color)
					m_pDevice->getDevice()->CreateRenderTargetView((itr++)->getResource(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_ColorTargetHeap->GetCPUDescriptorHandleForHeapStart(), colorIndex + 0, m_ColorHeapSize));

				if (attachmentTypes & AttachmentType::EntityID)
					m_pDevice->getDevice()->CreateRenderTargetView((itr++)->getResource(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_ColorTargetHeap->GetCPUDescriptorHandleForHeapStart(), colorIndex + 1, m_ColorHeapSize));

				if (attachmentTypes & AttachmentType::Normal)
					m_pDevice->getDevice()->CreateRenderTargetView((itr++)->getResource(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_ColorTargetHeap->GetCPUDescriptorHandleForHeapStart(), colorIndex + 2, m_ColorHeapSize));

				// Bind the depth and/ or stencil attachments.
				if (attachmentTypes & AttachmentType::Depth && attachmentTypes & AttachmentType::Stencil)
					m_pDevice->getDevice()->CreateDepthStencilView((itr++)->getResource(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DepthTargetHeap->GetCPUDescriptorHandleForHeapStart(), i + 0, m_DepthHeapSize));

				else if (attachmentTypes & AttachmentType::Depth)
					m_pDevice->getDevice()->CreateDepthStencilView((itr++)->getResource(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DepthTargetHeap->GetCPUDescriptorHandleForHeapStart(), i + 0, m_DepthHeapSize));

				else if (attachmentTypes & AttachmentType::Stencil)
					m_pDevice->getDevice()->CreateDepthStencilView((itr++)->getResource(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DepthTargetHeap->GetCPUDescriptorHandleForHeapStart(), i + 0, m_DepthHeapSize));
			}
		}

		Xenon::Backend::Image* DX12Rasterizer::getImageAttachment(AttachmentType type)
		{
			OPTICK_EVENT();

			const auto index = getAttachmentIndex(type);
			if (index < m_RenderTargets.size())
				return &m_RenderTargets[index];

			XENON_LOG_FATAL("The requested attachment type is not present in the rasterizer!");
			return nullptr;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DX12Rasterizer::getColorTargetHeapStartCPU() const
		{
			OPTICK_EVENT();

			return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_ColorTargetHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<UINT>(m_FrameIndex * getColorTargetCount()), m_ColorHeapSize);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DX12Rasterizer::getColorTargetHeapStartCPU()
		{
			OPTICK_EVENT();

			return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_ColorTargetHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<UINT>(m_FrameIndex * getColorTargetCount()), m_ColorHeapSize);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DX12Rasterizer::getDepthTargetHeapStartCPU()
		{
			OPTICK_EVENT();

			return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DepthTargetHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_DepthHeapSize);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DX12Rasterizer::getDepthTargetHeapStartCPU() const
		{
			OPTICK_EVENT();

			return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DepthTargetHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_DepthHeapSize);
		}

		uint8_t DX12Rasterizer::getAttachmentIndex(AttachmentType type) const
		{
			OPTICK_EVENT();

			if (m_AttachmentTypes & type)
			{
				uint8_t index = 0;
				if (m_AttachmentTypes & AttachmentType::Color)
				{
					if (type & AttachmentType::Color)
						return index;

					index++;
				}

				if (m_AttachmentTypes & AttachmentType::EntityID)
				{
					if (type & AttachmentType::EntityID)
						return index;

					index++;
				}

				if (m_AttachmentTypes & AttachmentType::Normal)
				{
					if (type & AttachmentType::Normal)
						return index;

					index++;
				}

				if (m_AttachmentTypes & AttachmentType::Depth && m_AttachmentTypes & AttachmentType::Stencil)
				{
					if (type & AttachmentType::Depth && type & AttachmentType::Stencil)
						return index;

					index++;
				}

				if (m_AttachmentTypes & AttachmentType::Depth)
				{
					if (type & AttachmentType::Depth)
						return index;

					index++;
				}

				if (m_AttachmentTypes & AttachmentType::Stencil)
				{
					if (type & AttachmentType::Stencil)
						return index;

					index++;
				}

				return index;
			}

			return -1;
		}

		void DX12Rasterizer::setupRenderTargets()
		{
			ImageSpecification specification;
			specification.m_Width = m_pCamera->getWidth();
			specification.m_Height = m_pCamera->getHeight();
			specification.m_EnableMipMaps = false;

			D3D12_CLEAR_VALUE colorOptimizedClearValue = {};
			colorOptimizedClearValue.Color[0] = 0.0f;
			colorOptimizedClearValue.Color[1] = 0.0f;
			colorOptimizedClearValue.Color[2] = 0.0f;
			colorOptimizedClearValue.Color[3] = 1.0f;

			if (m_AttachmentTypes & AttachmentType::Color)
			{
				specification.m_Usage = ImageUsage::ColorAttachment | ImageUsage::Storage;
				specification.m_Format = getBestFormat({ DataFormat::R8G8B8A8_UNORMAL, DataFormat::R8G8B8A8_SRGB }, D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
				specification.m_MultiSamplingCount = m_MultiSamplingCount;

				if (specification.m_Format == DataFormat::Undefined)
				{
					XENON_LOG_FATAL("The required color attachment formats are not supported by the Direct X 12 backend! Failed to create the render target.");
					return;
				}

				colorOptimizedClearValue.Format = DX12Device::ConvertFormat(specification.m_Format);
				m_RenderTargets.emplace_back(m_pDevice, specification, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &colorOptimizedClearValue);
				specification.m_MultiSamplingCount = MultiSamplingCount::x1;
			}

			if (m_AttachmentTypes & AttachmentType::EntityID)
			{
				specification.m_Usage = ImageUsage::ColorAttachment | ImageUsage::Storage;
				specification.m_Format = getBestFormat({ DataFormat::R32_SFLOAT, DataFormat::R16_SFLOAT, DataFormat::R8_SRGB }, D3D12_FORMAT_SUPPORT1_RENDER_TARGET);

				if (specification.m_Format == DataFormat::Undefined)
				{
					XENON_LOG_FATAL("The required entity ID attachment formats are not supported by the Direct X 12 backend! Failed to create the render target.");
					return;
				}

				colorOptimizedClearValue.Format = DX12Device::ConvertFormat(specification.m_Format);
				m_RenderTargets.emplace_back(m_pDevice, specification, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &colorOptimizedClearValue);
			}

			if (m_AttachmentTypes & AttachmentType::Normal)
			{
				specification.m_Usage = ImageUsage::ColorAttachment | ImageUsage::Storage;
				specification.m_Format = getBestFormat({ DataFormat::R32G32B32_SFLOAT }, D3D12_FORMAT_SUPPORT1_RENDER_TARGET);

				if (specification.m_Format == DataFormat::Undefined)
				{
					XENON_LOG_FATAL("The required normal attachment formats are not supported by the Direct X 12 backend! Failed to create the render target.");
					return;
				}

				colorOptimizedClearValue.Format = DX12Device::ConvertFormat(specification.m_Format);
				m_RenderTargets.emplace_back(m_pDevice, specification, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &colorOptimizedClearValue);
			}

			D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
			depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
			depthOptimizedClearValue.DepthStencil.Stencil = 0;

			if (m_AttachmentTypes & AttachmentType::Depth && m_AttachmentTypes & AttachmentType::Stencil)
			{
				specification.m_Usage = ImageUsage::DepthAttachment | ImageUsage::Storage;
				specification.m_Format = getBestFormat({ DataFormat::D24_UNORMAL_S8_UINT, DataFormat::D32_SFLOAT_S8_UINT }, D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);

				if (specification.m_Format == DataFormat::Undefined)
				{
					XENON_LOG_FATAL("The required normal attachment formats are not supported by the Direct X 12 backend! Failed to create the render target.");
					return;
				}

				depthOptimizedClearValue.Format = DX12Device::ConvertFormat(specification.m_Format);
				m_RenderTargets.emplace_back(m_pDevice, specification, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &depthOptimizedClearValue);
			}

			else if (m_AttachmentTypes & AttachmentType::Depth)
			{
				specification.m_Usage = ImageUsage::DepthAttachment | ImageUsage::Storage;
				specification.m_Format = getBestFormat({ DataFormat::D16_SINT, DataFormat::D32_SFLOAT }, D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);

				if (specification.m_Format == DataFormat::Undefined)
				{
					XENON_LOG_FATAL("The required normal attachment formats are not supported by the Direct X 12 backend! Failed to create the render target.");
					return;
				}

				depthOptimizedClearValue.Format = DX12Device::ConvertFormat(specification.m_Format);
				m_RenderTargets.emplace_back(m_pDevice, specification, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &depthOptimizedClearValue);
			}

			else if (m_AttachmentTypes & AttachmentType::Stencil)
			{
				specification.m_Usage = ImageUsage::DepthAttachment | ImageUsage::Storage;
				specification.m_Format = getBestFormat({ DataFormat::S8_UINT }, D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);

				if (specification.m_Format == DataFormat::Undefined)
				{
					XENON_LOG_FATAL("The required normal attachment formats are not supported by the Direct X 12 backend! Failed to create the render target.");
					return;
				}

				depthOptimizedClearValue.Format = DX12Device::ConvertFormat(specification.m_Format);
				m_RenderTargets.emplace_back(m_pDevice, specification, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &depthOptimizedClearValue);
			}
		}

		Xenon::Backend::DataFormat DX12Rasterizer::getBestFormat(const std::vector<DataFormat>& candidates, D3D12_FORMAT_SUPPORT1 formatSupport) const
		{
			const auto required = D3D12_FORMAT_SUPPORT1_TEXTURE2D | formatSupport;
			for (const auto format : candidates)
			{
				if (m_pDevice->getFormatSupport(DX12Device::ConvertFormat(format), required).first)
					return format;
			}

			return DataFormat::Undefined;
		}
	}
}