// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Device.hpp"
#include "../XenonBackend/ShaderSource.hpp"

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

			/**
			 * Finish all device operations and wait idle.
			 */
			void waitIdle() override;

		public:
			/**
			 * Convert the Xenon format to the Direct X 12 format.
			 *
			 * @param format The Xenon format.
			 * @return The DX12 format.
			 */
			[[nodiscard]] DXGI_FORMAT convertFormat(DataFormat format) const;

			/**
			 * Get the format support.
			 *
			 * @param format The format to check for support.
			 * @param support1 The format support 1. Default is none.
			 * @param support2 The format support 2. Default is none.
			 * @return The format support pair. The first one is for the support 1 and second is for support 2.
			 */
			[[nodiscard]] std::pair<bool, bool> getFormatSupport(DXGI_FORMAT format, D3D12_FORMAT_SUPPORT1 support1 = D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2 support2 = D3D12_FORMAT_SUPPORT2_NONE) const;

			/**
			 * Compile a shader to DirectX format.
			 *
			 * @param shader The shader to compile.
			 * @param type The shader type.
			 * @param newEntryPoint The new entry point name. If this is not a null string, it'll rename the current entry point to this.
			 * @return The compiled shader blob.
			 */
			[[nodiscard]] static ComPtr<ID3DBlob> CompileShader(const ShaderSource& shader, ShaderType type, const std::string_view& newEntryPoint = "");

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
			 * Get the device factory.
			 *
			 * @return The factory pointer.
			 */
			[[nodiscard]] IDXGIFactory4* getFactory() { return m_Factory.Get(); }

			/**
			 * Get the device factory.
			 *
			 * @return The const factory pointer.
			 */
			[[nodiscard]] const IDXGIFactory4* getFactory() const { return m_Factory.Get(); }

			/**
			 * Get the backend device object.
			 *
			 * @return The device pointer.
			 */
			[[nodiscard]] ID3D12Device5* getDevice() { return m_Device.Get(); }

			/**
			 * Get the backend device object.
			 *
			 * @return The const device pointer.
			 */
			[[nodiscard]] const ID3D12Device5* getDevice() const { return m_Device.Get(); }

			/**
			 * Get the device adapter.
			 *
			 * @return The adapter pointer.
			 */
			[[nodiscard]] IDXGIAdapter* getAdapter() { return m_Adapter.Get(); }

			/**
			 * Get the device adapter.
			 *
			 * @return The adapter pointer.
			 */
			[[nodiscard]] const IDXGIAdapter* getAdapter() const { return m_Adapter.Get(); }

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
			 * Get the direct command queue.
			 *
			 * @return The direct command queue pointer.
			 */
			[[nodiscard]] ID3D12CommandQueue* getDirectQueue() { return m_DirectQueue.Get(); }

			/**
			 * Get the direct queue.
			 *
			 * @return The const direct queue pointer.
			 */
			[[nodiscard]] const ID3D12CommandQueue* getDirectQueue() const { return m_DirectQueue.Get(); }

			/**
			 * Get the bundle command queue.
			 *
			 * @return The bundle command queue pointer.
			 */
			[[nodiscard]] ID3D12CommandQueue* getBundleQueue() { return m_BundleQueue.Get(); }

			/**
			 * Get the bundle queue.
			 *
			 * @return The const bundle queue pointer.
			 */
			[[nodiscard]] const ID3D12CommandQueue* getBundleQueue() const { return m_BundleQueue.Get(); }

			/**
			 * Get the copy queue.
			 *
			 * @return The command queue pointer.
			 */
			[[nodiscard]] ID3D12CommandQueue* getCopyQueue() { return m_CopyQueue.Get(); }

			/**
			 * Get the copy queue.
			 *
			 * @return The const command queue pointer.
			 */
			[[nodiscard]] const ID3D12CommandQueue* getCopyQueue() const { return m_CopyQueue.Get(); }


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
			ComPtr<ID3D12Device5> m_Device;
			ComPtr<IDXGIAdapter> m_Adapter;

			ComPtr<ID3D12CommandQueue> m_DirectQueue;
			ComPtr<ID3D12CommandQueue> m_BundleQueue;
			ComPtr<ID3D12CommandQueue> m_CopyQueue;

			D3D12MA::Allocator* m_pAllocator = nullptr;
		};
	}
}