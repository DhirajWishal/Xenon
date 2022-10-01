// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Device.hpp"

#include "DX12Instance.hpp"

#include <D3D12MemAlloc.h>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 device class.
		 */
		class DX12Device final : public Device
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pInstance The instance pointer.
			 * @param requiredRenderTargets The required render targets by this device.
			 */
			explicit DX12Device(DX12Instance* pInstance, RenderTargetType requiredRenderTargets);

			/**
			 * Destructor.
			 */
			~DX12Device() override;

		public:
			/**
			 * Get the backend device object.
			 *
			 * @return The device pointer.
			 */
			[[nodiscard]] ID3D12Device* getDevice() { return m_Device.Get(); }

			/**
			 * Get the backend device object.
			 *
			 * @return The const device pointer.
			 */
			[[nodiscard]] const ID3D12Device* getDevice() const { return m_Device.Get(); }

		private:
			/**
			 * Create the DXGI factory.
			 */
			void createFactory();

			/**
			 * Create the DirectX 12 device.
			 */
			void createDevice();

			/**
			 * Create the required queues.
			 */
			void createQueue();

			/**
			 * Create the D3D12MA allocator.
			 */
			void createAllocator();

			/**
			 * Get a suitable hardware adapter.
			 *
			 * @param pFactory The factory pointer.
			 * @param ppAdapter The pointer to the adapter pointer.
			 * @param featureLevel The feature level.
			 */
			void getHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, D3D_FEATURE_LEVEL featureLevel);

		private:
			DX12Instance* m_pInstance = nullptr;

			ComPtr<IDXGIFactory4> m_Factory;
			ComPtr<ID3D12Device> m_Device;
			ComPtr<IDXGIAdapter> m_Adapter;
			ComPtr<ID3D12CommandQueue> m_GraphicsQueue;

			D3D12MA::Allocator* m_pAllocator = nullptr;
		};
	}
}