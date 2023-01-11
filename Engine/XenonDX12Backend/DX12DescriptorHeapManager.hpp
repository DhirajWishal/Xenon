// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/DescriptorManager.hpp"
#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 pipeline descriptor heap storage type.
		 * This vector can contain 2 possible entries.
		 * 1. Index 0 contains the CBV, SRV and UAV descriptor heap.
		 * 2. Index 1 contains the Sampler descriptor heap.
		 *
		 * The heaps in the vector are freshly copied from a non shader-visible heap to a shader-visible descriptor heap(s) which can be attached to a command list
		 * to be executed using shaders.
		 */
		using DX12PipelineDescriptorHeapStorage = std::vector<ID3D12DescriptorHeap*>;

		/**
		 * DirectX 12 descriptor heap manager class.
		 * This object is used to manage descriptor heaps and create new heaps if needed.
		 */
		class DX12DescriptorHeapManager final : public DescriptorManager, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit DX12DescriptorHeapManager(DX12Device* pDevice);

			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param type The descriptor type.
			 * @param bindingInfo The descriptor's binding info.
			 */
			explicit DX12DescriptorHeapManager(DX12Device* pDevice, DescriptorType type, const std::vector<DescriptorBindingInfo>& bindingInfo);

			/**
			 * Create a new descriptor.
			 *
			 * @return The created descriptor.
			 */
			[[nodiscard]] std::unique_ptr<Descriptor> create() override;

			/**
			 * Setup the descriptor heap(s).
			 *
			 * @param bindingMap The descriptor binding map.
			 */
			void setupDescriptorHeapManager(std::unordered_map<DescriptorType, std::vector<DescriptorBindingInfo>>&& bindingMap);

			/**
			 * Get the descriptor heap storage.
			 * This will perform an internal copy if the descriptor heaps were updated before this call.
			 *
			 * @return The storage reference.
			 */
			[[nodiscard]] DX12PipelineDescriptorHeapStorage& getDescriptorHeapStorage();

			/**
			 * Get the binding infos.
			 *
			 * @param type The descriptor type.
			 * @return The binding infos.
			 */
			[[nodiscard]] const std::vector<DescriptorBindingInfo>& getBindingInfo(DescriptorType type) const { return m_BindingMap.at(type); }

			/**
			 * Setup a new descriptor
			 *
			 * @param type The descriptor type.
			 * @return First contains the CBV, SRV and UAV count.
			 * @return Second contains the Sampler count.
			 */
			[[nodiscard]] std::pair<UINT, UINT> setupDescriptor(DescriptorType type);

			/**
			 * Get the heap group size pair.
			 *
			 * @param type The descriptor type.
			 * @return First contains the CBV, SRV and UAV count.
			 * @return Second contains the Sampler count.
			 */
			[[nodiscard]] std::pair<UINT, UINT> getGroupSize(DescriptorType type) const { return m_GroupSizes.at(type); }

			/**
			 * Free the descriptor.
			 *
			 * @param cbvUavSrvStart The start of the CBV, UAV, SRV descriptor.
			 * @param samplerStart The start of the sampler descriptor.
			 */
			void freeDescriptor(UINT cbvUavSrvStart, UINT samplerStart) noexcept;

			/**
			 * Get the sampler index.
			 *
			 * @param index The index to access.
			 * @return The sampler index.
			 */
			[[nodiscard]] UINT getSamplerIndex(uint32_t index) const { return m_SamplerIndex[index]; }

			/**
			 * Notify the manager that the heap has been updated.
			 */
			void notifyHeapUpdated() { m_IsUpdated = true; }

			/**
			 * Get the CPU CBV, SRV and UAV heap start.
			 *
			 * @return The descriptor handle.
			 */
			[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCbvSrvUavHeapStartCPU() const { return m_CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(); }

			/**
			 * Get the GPU CBV, SRV and UAV heap start.
			 *
			 * @return The descriptor handle.
			 */
			[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getCbvSrvUavHeapStartGPU() const { return m_CbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart(); }

			/**
			 * Get the CBV, SRV and UAV heap increment size.
			 *
			 * @return The increment size.
			 */
			[[nodiscard]] UINT getCbvSrvUavHeapIncrementSize() const { return m_CbvSrvUavHeapIncrementSize; }

			/**
			 * Get the CPU sampler heap start.
			 *
			 * @return The descriptor handle.
			 */
			[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getSamplerHeapStartCPU() const { return m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart(); }

			/**
			 * Get the GPU sampler heap start.
			 *
			 * @return The descriptor handle.
			 */
			[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getSamplerHeapStartGPU() const { return m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart(); }

			/**
			 * Get the sampler heap increment size.
			 *
			 * @return The increment size.
			 */
			[[nodiscard]] UINT getSamplerHeapIncrementSize() const { return m_SamplerHeapIncrementSize; }

		private:
			/**
			 * Calculate the next needed size.
			 *
			 * @param newSize The required size.
			 * @param oldSize The old size of the heap.
			 * @return The new size.
			 */
			UINT getNextSize(UINT newSize, UINT oldSize) const;

			/**
			 * Increment the heaps.
			 */
			void incrementHeaps();

		public:
			std::mutex m_Mutex;

			std::unordered_map<DescriptorType, std::vector<DescriptorBindingInfo>> m_BindingMap;
			std::unordered_map<DescriptorType, std::pair<UINT, UINT>> m_GroupSizes;	// [buffer, sampler]

			std::vector<UINT> m_SamplerIndex;
			std::vector<CD3DX12_DESCRIPTOR_RANGE1> m_Ranges;
			std::vector<std::pair<UINT, UINT>> m_ReUsableDescriptors;

			DX12PipelineDescriptorHeapStorage m_ShaderVisibleHeaps{};

			ComPtr<ID3D12DescriptorHeap> m_CbvSrvUavDescriptorHeap;
			ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap;

			ComPtr<ID3D12DescriptorHeap> m_ShaderVisibleCbvSrvUavDescriptorHeap;
			ComPtr<ID3D12DescriptorHeap> m_ShaderVisibleSamplerDescriptorHeap;

			UINT m_CbvSrvUavHeapIncrementSize = 0;
			UINT m_SamplerHeapIncrementSize = 0;

			UINT m_CbvSrvUavDescriptorCount = 0;
			UINT m_SamplerDescriptorCount = 0;

			UINT m_AllocatedCbvSrvUavDescriptorCount = 0;
			UINT m_AllocatedSamplerDescriptorCount = 0;

			UINT m_CbvSrvUavCount = 0;
			UINT m_SamplerCount = 0;

			bool m_IsUpdated = true;
		};
	}
}