// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 acceleration structure class.
		 * This is the base class for all the DirectX 12 acceleration structures.
		 */
		class DX12AccelerationStructure : public DX12DeviceBoundObject
		{
		public:
			/**
			 * Default constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit DX12AccelerationStructure(DX12Device* pDevice);

			/**
			 * Virtual destructor.
			 */
			virtual ~DX12AccelerationStructure() override;

			/**
			 * Get the acceleration structure's GPU virtual address.
			 *
			 * @return The GPU virtual address.
			 */
			[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS getAccelerationStructureGPUVirtualAddress() const noexcept { return m_pAccelerationStructure->GetResource()->GetGPUVirtualAddress(); }

		protected:
			/**
			 * Create the scratch buffer.
			 *
			 * @param size The size of the scratch buffer.
			 */
			void createScratchBuffer(UINT64 size);

			/**
			 * Create the acceleration structure.
			 *
			 * @param size The size of the acceleration structure.
			 */
			void createAccelerationStructure(UINT64 size);

		protected:
			D3D12MA::Allocation* m_pScratchBuffer = nullptr;
			D3D12MA::Allocation* m_pAccelerationStructure = nullptr;
			UINT64 m_ResultDataMaxSizeInBytes = 0;
		};
	}
}