// Copyright 2022 Dhiraj Wishal
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
			D3D12MA::Allocation* m_pAllocation = nullptr;
		};
	}
}