// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Rasterizer.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Rasterizer::DX12Rasterizer(DX12Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering /*= false*/, MultiSamplingCount multiSampleCount /*= MultiSamplingCount::x1*/)
			: DX12DeviceBoundObject(pDevice)
			, Rasterizer(pDevice, pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount)
			, m_pDevice(pDevice)
		{
			const auto frameCount = m_bEnableTripleBuffering ? 3 : 1;

			// Describe and create a render target view (RTV) descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = frameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RenderTargetHeap)), "Failed to create the Render Target View heap!");

			// Describe and create a shader resource view (SRV) heap for the texture.
			D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
			srvHeapDesc.NumDescriptors = 1;
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_RenderTargetShaderViewHeap)), "Failed to create the Render Target Shader View heap!");

			m_RenderTargetDescriptorSize = m_pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RenderTargetHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a RTV for each frame.
			for (UINT i = 0; i < frameCount; i++)
			{
				ImageSpecification specification;
				specification.m_Width = pCamera->getWidth();
				specification.m_Height = pCamera->getHeight();
				specification.m_EnableMipMaps = false;
				specification.m_Usage = ImageUsage::ColorAttachment | ImageUsage::Storage;

				// Check if the required formats are supported.
				const auto required = D3D12_FORMAT_SUPPORT1_TEXTURE2D | D3D12_FORMAT_SUPPORT1_RENDER_TARGET;
				auto formatSupport = m_pDevice->getFormatSupport(DXGI_FORMAT_R8G8B8A8_UNORM);
				if ((formatSupport.Support1 & required) != required)
				{
					formatSupport = m_pDevice->getFormatSupport(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
					if ((formatSupport.Support1 & required) != required)
					{
						XENON_LOG_FATAL("The required render target formats are not supported by the Direct X 12 backend! Failed to create the render target.");
						return;
					}

					specification.m_Format = DataFormat::R8G8B8A8_SRGB;
				}
				else
				{
					specification.m_Format = DataFormat::R8G8B8A8_UNORMAL;
				}

				auto& renderTarget = m_RenderTargets.emplace_back(pDevice, specification, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES);

				// XENON_DX12_ASSERT(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[i])));
				m_pDevice->getDevice()->CreateRenderTargetView(renderTarget.getResource(), nullptr, rtvHandle);
				rtvHandle.Offset(1, m_RenderTargetDescriptorSize);
			}
		}
	}
}