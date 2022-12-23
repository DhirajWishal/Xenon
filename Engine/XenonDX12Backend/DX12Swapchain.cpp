// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Swapchain.hpp"
#include "DX12Macros.hpp"

#include "../XenonPlatformWindows/WindowsWindow.hpp"

#include <optick.h>
#include <glm/vec2.hpp>

namespace Xenon
{
	namespace Backend
	{
		DX12Swapchain::DX12Swapchain(DX12Device* pDevice, const std::string& title, uint32_t width, uint32_t height)
			: Swapchain(pDevice, title, width, height)
			, DX12DeviceBoundObject(pDevice)
		{
			m_FrameCount = 3;	XENON_TODO(0, 0, 0, "(Dhiraj) Find a better system.");
			m_SwapChainFormat = getBestSwapchainFormat();

			// Create the swapchain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.BufferCount = m_FrameCount;
			swapChainDesc.Width = width;
			swapChainDesc.Height = height;
			swapChainDesc.Format = m_SwapChainFormat;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.SampleDesc.Count = 1;

			const auto pWindowsWindow = static_cast<Platform::WindowsWindow*>(m_pWindow.get());
			const auto windowHandle = pWindowsWindow->getWindowHandle();

			ComPtr<IDXGISwapChain1> swapChain;
			XENON_DX12_ASSERT(pDevice->getFactory()->CreateSwapChainForHwnd(
				pDevice->getDirectQueue(),
				windowHandle,
				&swapChainDesc,
				nullptr,
				nullptr,
				&swapChain
			), "Failed to create the swapchain!");

			// This sample does not support full screen transitions.
			XENON_DX12_ASSERT(pDevice->getFactory()->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER), "Failed to make the window association!");
			XENON_DX12_ASSERT(swapChain.As(&m_SwapChain), "Failed to assign the swapchain!");

			// Create the swapchain image heap.
			D3D12_DESCRIPTOR_HEAP_DESC swapchainImageHeapDesc = {};
			swapchainImageHeapDesc.NumDescriptors = m_FrameCount;
			swapchainImageHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			swapchainImageHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateDescriptorHeap(&swapchainImageHeapDesc, IID_PPV_ARGS(&m_SwapchainImageHeap)), "Failed to create the swapchain image heap!");
			XENON_DX12_NAME_OBJECT(m_SwapchainImageHeap, "Swapchain Image Heap");

			// Get the heap descriptor size.
			m_SwapchainImageHeapDescriptorSize = pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			// Get the swapchain images.
			CD3DX12_CPU_DESCRIPTOR_HANDLE swapchainImageHeapHandle(m_SwapchainImageHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a Swapchain image for each frame.
			m_SwapchainImages.resize(m_FrameCount);
			for (UINT i = 0; i < m_FrameCount; i++)
			{
				XENON_DX12_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapchainImages[i])), "Failed to get the swapchain back buffer!");
				pDevice->getDevice()->CreateRenderTargetView(m_SwapchainImages[i].Get(), nullptr, swapchainImageHeapHandle);
				swapchainImageHeapHandle.Offset(1, m_SwapchainImageHeapDescriptorSize);

				XENON_DX12_NAME_OBJECT(m_SwapchainImages[i], "Swapchain Image");
			}

			// Create the fence.
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_FrameFence)), "Failed to create the frame fence!");
			XENON_DX12_NAME_OBJECT(m_FrameFence, "Swapchain Frame Fence");
			m_FenceValues.resize(m_FrameCount);

			// Setup the image copy container.
			setupImageCopyContainer();
		}

		uint32_t DX12Swapchain::prepare()
		{
			OPTICK_EVENT();

			return m_ImageIndex;
		}

		void DX12Swapchain::present()
		{
			OPTICK_EVENT();

			// Present the swapchain.
			DXGI_PRESENT_PARAMETERS parameters = { 0 };
			XENON_DX12_ASSERT(m_SwapChain->Present1(1, 0, &parameters), "Failed to present the swapchain!");

			// Move to the next frame.
			// Schedule a Signal command in the queue.
			const UINT64 currentFenceValue = m_FenceValues[m_ImageIndex];
			XENON_DX12_ASSERT(m_pDevice->getDirectQueue()->Signal(m_FrameFence.Get(), currentFenceValue), "Failed to signal the command queue!");

			// Update the frame index.
			m_ImageIndex = m_SwapChain->GetCurrentBackBufferIndex();

			// If the next frame is not ready to be rendered yet, wait until it is ready.
			if (m_FrameFence->GetCompletedValue() < m_FenceValues[m_ImageIndex])
			{
				XENON_DX12_ASSERT(m_FrameFence->SetEventOnCompletion(m_FenceValues[m_ImageIndex], m_FenceEvent), "Failed t set the event on completion to the fence event!");
				WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
			}

			// Set the fence value for the next frame.
			m_FenceValues[m_ImageIndex] = currentFenceValue + 1;

			// Increment the frame index.
			incrementFrame();
		}

		void DX12Swapchain::recreate()
		{
			XENON_TODO_NOW("(Dhiraj) Implement this function.");
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DX12Swapchain::getCPUDescriptorHandle() const
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_SwapchainImageHeap->GetCPUDescriptorHandleForHeapStart(), m_ImageIndex, m_SwapchainImageHeapDescriptorSize);
		}

		void DX12Swapchain::prepareDescriptorForImageCopy(DX12Image* pImage)
		{
			OPTICK_EVENT();

			// Skip if we have already created the required resource view for the image.
			if (m_ImageCopyContainer.m_pPreviousColorImage == pImage)
				return;

			// Else setup the resource view.
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = m_pDevice->convertFormat(pImage->getDataFormat());
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			m_pDevice->getDevice()->CreateShaderResourceView(pImage->getResource(), &srvDesc, m_ImageCopyContainer.m_CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		}

		DXGI_FORMAT DX12Swapchain::getBestSwapchainFormat() const
		{
			constexpr auto candidates = std::array{
				DXGI_FORMAT_R10G10B10A2_UNORM,

				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
				DXGI_FORMAT_B8G8R8A8_UNORM,
				DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,

				DXGI_FORMAT_R32G32B32A32_FLOAT,
				DXGI_FORMAT_R32G32B32A32_UINT,
				DXGI_FORMAT_R32G32B32A32_SINT,

				DXGI_FORMAT_R16G16B16A16_FLOAT,

				DXGI_FORMAT_NV12,
				DXGI_FORMAT_YUY2
			};

			// Pick the best candidate.
			for (const auto candidate : candidates)
			{
				if (m_pDevice->getFormatSupport(candidate, D3D12_FORMAT_SUPPORT1_RENDER_TARGET | D3D12_FORMAT_SUPPORT1_DISPLAY).first)
					return candidate;
			}

			XENON_LOG_FATAL("No suitable swapchain formats found!");
			return DXGI_FORMAT_UNKNOWN;
		}

		void DX12Swapchain::setupImageCopyContainer()
		{
			// Setup the descriptor heap for the incoming image.
			{
				// Setup the texture descriptor.
				D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
				srvHeapDesc.NumDescriptors = 1;
				srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_ImageCopyContainer.m_CbvSrvUavDescriptorHeap)), "Failed to create the image-to-swapchain copy CBV, SRV and UAV descriptor!");
				XENON_DX12_NAME_OBJECT(m_ImageCopyContainer.m_CbvSrvUavDescriptorHeap, "Swapchain Image Copy CBV, SRV and UAV Descriptor Heap");

				// Setup the sampler descriptor.
				D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
				samplerHeapDesc.NumDescriptors = 1;
				samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
				samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_ImageCopyContainer.m_SamplerDescriptorHeap)), "Failed to create the image-to-swapchain copy sampler descriptor!");
				XENON_DX12_NAME_OBJECT(m_ImageCopyContainer.m_SamplerDescriptorHeap, "Swapchain Image Copy Sampler Descriptor Heap");

				// Setup the sampler to improve performance.
				D3D12_SAMPLER_DESC sampler = {};
				sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
				sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
				sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
				sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
				sampler.MipLODBias = 0;
				sampler.MaxAnisotropy = 0;
				sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
				sampler.BorderColor[0] = 0.0f;
				sampler.BorderColor[1] = 0.0f;
				sampler.BorderColor[2] = 0.0f;
				sampler.BorderColor[3] = 0.0f;
				sampler.MinLOD = 0.0f;
				sampler.MaxLOD = D3D12_FLOAT32_MAX;
				m_pDevice->getDevice()->CreateSampler(&sampler, m_ImageCopyContainer.m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			}

			// Setup the root signature.
			{
				D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

				// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

				if (FAILED(m_pDevice->getDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
					featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

				const std::array<D3D12_DESCRIPTOR_RANGE1, 2> range = { CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0), CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0) };
				std::array<CD3DX12_ROOT_PARAMETER1, 2> rootParameters = {};

				CD3DX12_ROOT_PARAMETER1 rootParameter = {};
				rootParameter.InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_PIXEL);
				rootParameters[0] = rootParameter;

				rootParameter.InitAsDescriptorTable(1, &range[1], D3D12_SHADER_VISIBILITY_PIXEL);
				rootParameters[1] = rootParameter;

				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
				rootSignatureDesc.Init_1_1(static_cast<UINT>(rootParameters.size()), rootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

				ComPtr<ID3DBlob> signature;
				ComPtr<ID3DBlob> error;
				XENON_DX12_ASSERT(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error), "Failed to serialize the version-ed root signature for the I2SC root signature!");
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_ImageCopyContainer.m_RootSignature)), "Failed to create the I2SC root signature!");
				XENON_DX12_NAME_OBJECT(m_ImageCopyContainer.m_RootSignature, "Swapchain Image Copy Root Signature");
			}

			/**
			 * Vertex structure.
			 */
			struct Vertex final
			{
				glm::vec2 m_Position;
				glm::vec2 m_UV;
			};

			// Setup the pipeline state.
			{
				ComPtr<ID3DBlob> vertexShader = DX12Device::CompileShader(ShaderSource::FromFile(XENON_SHADER_DIR "Internal/DX12SwapchainCopy/Shader.vert.spv"), ShaderType::Vertex);
				ComPtr<ID3DBlob> pixelShader = DX12Device::CompileShader(ShaderSource::FromFile(XENON_SHADER_DIR "Internal/DX12SwapchainCopy/Shader.frag.spv"), ShaderType::Fragment);

				// Define the vertex input layout.
				constexpr std::array<D3D12_INPUT_ELEMENT_DESC, 2> inputElementDescs = {
					D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, m_Position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, m_UV), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
				};

				// Describe and create the graphics pipeline state object (PSO).
				D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
				psoDesc.InputLayout = { inputElementDescs.data(), inputElementDescs.size() };
				psoDesc.pRootSignature = m_ImageCopyContainer.m_RootSignature.Get();
				psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
				psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
				psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
				psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
				psoDesc.DepthStencilState.DepthEnable = FALSE;
				psoDesc.DepthStencilState.StencilEnable = FALSE;
				psoDesc.SampleMask = UINT_MAX;
				psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				psoDesc.NumRenderTargets = 1;
				psoDesc.RTVFormats[0] = m_SwapChainFormat;
				psoDesc.SampleDesc.Count = 1;

				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_ImageCopyContainer.m_PipelineState)), "Failed to create the I2SC pipeline state object!");
				XENON_DX12_NAME_OBJECT(m_ImageCopyContainer.m_PipelineState, "Swapchain Image Copy Pipeline State");
			}

			// Setup the vertex data.
			{
				// Define the geometry for a triangle.
				const std::array<Vertex, 6> triangleVertices =
				{
					Vertex{.m_Position = glm::vec2(1.0f, 1.0f), .m_UV = glm::vec2(1.0f, 0.0f)},
					Vertex{.m_Position = glm::vec2(1.0f, -1.0f), .m_UV = glm::vec2(1.0f, 1.0f)},
					Vertex{.m_Position = glm::vec2(-1.0f, -1.0f), .m_UV = glm::vec2(0.0f, 1.0f)},

					Vertex{.m_Position = glm::vec2(-1.0f, -1.0f), .m_UV = glm::vec2(0.0f, 1.0f)},
					Vertex{.m_Position = glm::vec2(-1.0f, 1.0f), .m_UV = glm::vec2(0.0f, 0.0f)},
					Vertex{.m_Position = glm::vec2(1.0f, 1.0f), .m_UV = glm::vec2(1.0f, 0.0f)},
				};

				const UINT vertexBufferSize = sizeof(Vertex) * triangleVertices.size();

				const D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				const D3D12_RESOURCE_DESC resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommittedResource(
					&heapProperties,
					D3D12_HEAP_FLAG_NONE,
					&resourceDescriptor,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_ImageCopyContainer.m_VertexBuffer)), "Failed to create the I2SC vertex buffer!");

				XENON_DX12_NAME_OBJECT(m_ImageCopyContainer.m_VertexBuffer, "Swapchain Image Copy Vertex Buffer");

				// Copy the triangle data to the vertex buffer.
				UINT8* pVertexDataBegin = nullptr;
				const CD3DX12_RANGE readRange(0, 0);
				XENON_DX12_ASSERT(m_ImageCopyContainer.m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)), "Failed to map the I2SC vertex buffer!");
				memcpy(pVertexDataBegin, triangleVertices.data(), vertexBufferSize);
				m_ImageCopyContainer.m_VertexBuffer->Unmap(0, nullptr);

				// Initialize the vertex buffer view.
				m_ImageCopyContainer.m_VertexBufferView.BufferLocation = m_ImageCopyContainer.m_VertexBuffer->GetGPUVirtualAddress();
				m_ImageCopyContainer.m_VertexBufferView.StrideInBytes = sizeof(Vertex);
				m_ImageCopyContainer.m_VertexBufferView.SizeInBytes = vertexBufferSize;
			}
		}
	}
}