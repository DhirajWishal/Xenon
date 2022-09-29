// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Device.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Device::DX12Device(DX12Instance* pInstance, RenderTargetType requiredRenderTargets)
			: Device(pInstance, requiredRenderTargets)
			, m_pInstance(pInstance)
		{
			// Create the factory.
			createFactory();

			// Create the device.
			createDevice();

			// Create the queues.
			createQueue();
		}

		void DX12Device::createFactory()
		{
			XENON_DX12_ASSERT(CreateDXGIFactory2(m_pInstance->getFactoryFlags(), IID_PPV_ARGS(&m_Factory)), "Failed to create the DXGI factory!");
		}

		void DX12Device::createDevice()
		{
			// Setup the test feature levels if a device was not found.
			constexpr D3D_FEATURE_LEVEL testFeatureLevels[] = {
				D3D_FEATURE_LEVEL_12_2,
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0
			};

			// Iterate over the features and check if the best feature is available.
			for (uint8_t i = 0; (i < 3) && !m_Device; i++)
			{
				const auto featureLevel = testFeatureLevels[i];

				// Create the hardware adapter.
				ComPtr<IDXGIAdapter1> hardwareAdapter;
				getHardwareAdapter(m_Factory.Get(), &hardwareAdapter, featureLevel);

				// Try to create the device.
				const auto result = D3D12CreateDevice(
					hardwareAdapter.Get(),
					featureLevel,
					IID_PPV_ARGS(&m_Device)
				);

				// If it failed, try making a software rasterizing device.
				if (FAILED(result))
				{
					ComPtr<IDXGIAdapter> warpAdapter;
					XENON_DX12_ASSERT(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)), "Failed to wrap the adapter!");

					D3D12CreateDevice(
						warpAdapter.Get(),
						featureLevel,
						IID_PPV_ARGS(&m_Device)
					);
				}
			}

			// Check if we were able to create a device.
			if (!m_Device)
				XENON_LOG_FATAL("Failed to create a DirectX device!");
		}

		void DX12Device::createQueue()
		{
			// Setup graphics queue.
			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			XENON_DX12_ASSERT(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_GraphicsQueue)), "Failed to create the DirectX 12 graphics queue!");
		}

		_Use_decl_annotations_ void DX12Device::getHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, D3D_FEATURE_LEVEL featureLevel)
		{
			*ppAdapter = nullptr;

			ComPtr<IDXGIAdapter1> adapter;

			// Create the factory to get the adapter.
			ComPtr<IDXGIFactory6> factory6;
			if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
			{
				for (UINT i = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter))); i++)
				{
					DXGI_ADAPTER_DESC1 desc;
					adapter->GetDesc1(&desc);

					// Skip the software renderer.
					if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
						continue;

					// Check to see whether the adapter supports the feature level.
					if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), featureLevel, _uuidof(ID3D12Device), nullptr)))
						break;
				}
			}

			// If we don't have an adapter, enumerate all the adapters and check.
			if (adapter.Get() == nullptr)
			{
				for (UINT i = 0; SUCCEEDED(pFactory->EnumAdapters1(i, &adapter)); i++)
				{
					DXGI_ADAPTER_DESC1 desc;
					adapter->GetDesc1(&desc);

					// Skip the software renderer.
					if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
						continue;

					// Check to see whether the adapter supports the feature level.
					if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), featureLevel, _uuidof(ID3D12Device), nullptr)))
						break;
				}
			}

			*ppAdapter = adapter.Detach();
		}
	}
}