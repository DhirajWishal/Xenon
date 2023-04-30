// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/ShaderBindingTable.hpp"

#include "DX12RayTracingPipeline.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 shader binding table class.
		 */
		class DX12ShaderBindingTable final : public ShaderBindingTable, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pPipeline The ray tracing pipeline pointer.
			 * @param bindingGroups The binding groups.
			 */
			explicit DX12ShaderBindingTable(DX12Device* pDevice, DX12RayTracingPipeline* pPipeline, const std::vector<BindingGroup>& bindingGroups);

			/**
			 * Destructor.
			 */
			~DX12ShaderBindingTable() override;

			/**
			 * Get the ray generation address range.
			 *
			 * @return The address range.
			 */
			[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS_RANGE getRayGenerationAddresRange() const noexcept { return m_RayGenerationAddressRange; }

			/**
			 * Get the miss shader address range.
			 *
			 * @return The address range and stride.
			 */
			[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getMissAddressRange() const noexcept { return m_MissAddressRange;}

			/**
			 * Get the hit group address range.
			 *
			 * @return The address range and stride.
			 */
			[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getHitGroupAddressRange() const noexcept { return m_HitGroupAddressRange;}

			/**
			 * Get the callable shader address range.
			 *
			 * @return The address range and stride.
			 */
			[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getCallableAddressRange() const noexcept { return m_CallableAddressRange;}

		private:
			/**
			 * Map the allocated memory to the CPU.
			 *
			 * @return The mapped memory.
			 */
			[[nodiscard]] std::byte* map();

			/**
			 * Unmap the mapped memory.
			 */
			void unmap();

		private:
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE m_RayGenerationAddressRange = {};
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE m_MissAddressRange = {};
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE m_HitGroupAddressRange = {};
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE m_CallableAddressRange = {};

			D3D12MA::Allocation* m_pAllocation = nullptr;
		};
	}
}