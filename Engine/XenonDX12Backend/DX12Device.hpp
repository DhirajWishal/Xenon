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
			 * Get the instance pointer.
			 *
			 * @return The instance pointer.
			 */
			[[nodiscard]] DX12Instance* getInstance() { return m_pInstance; }

			/**
			 * Get the instance pointer.
			 *
			 * @return The const instance pointer.
			 */
			[[nodiscard]] const DX12Instance* getInstance() const { return m_pInstance; }

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

			/**
			 * Get the memory allocator.
			 *
			 * @return The allocator.
			 */
			[[nodiscard]] D3D12MA::Allocator* getAllocator() { return m_pAllocator; }

			/**
			 * Get the memory allocator.
			 *
			 * @return The const allocator.
			 */
			[[nodiscard]] const D3D12MA::Allocator* getAllocator() const { return m_pAllocator; }

			/**
			 * Get the command queue.
			 *
			 * @return The command queue pointer.
			 */
			[[nodiscard]] ID3D12CommandQueue* getCommandQueue() { return m_GraphicsQueue.Get(); }

			/**
			 * Get the command queue.
			 *
			 * @return The const command queue pointer.
			 */
			[[nodiscard]] const ID3D12CommandQueue* getCommandQueue() const { return m_GraphicsQueue.Get(); }

			/**
			 * Get the global command allocator.
			 *
			 * @return The command allocator pointer.
			 */
			[[nodsicard]] ID3D12CommandAllocator* getCommandAllocator() { return m_GlobalCommandAllocator.Get(); }

			/**
			 * Get the global command allocator.
			 *
			 * @return The const command allocator pointer.
			 */
			[[nodsicard]] const ID3D12CommandAllocator* getCommandAllocator() const { return m_GlobalCommandAllocator.Get(); }

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
			 * Create the required command structures.
			 */
			void createCommandStructures();

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
			ComPtr<ID3D12CommandAllocator> m_GlobalCommandAllocator;

			D3D12MA::Allocator* m_pAllocator = nullptr;
		};
	}
}