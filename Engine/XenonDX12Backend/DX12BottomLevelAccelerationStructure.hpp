// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/BottomLevelAccelerationStructure.hpp"

#include "DX12Buffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 bottom level acceleration structure class.
		 */
		class DX12BottomLevelAccelerationStructure final : public BottomLevelAccelerationStructure, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param geometries The geometries to be stored in the acceleration structure.
			 */
			explicit DX12BottomLevelAccelerationStructure(DX12Device* pDevice, const std::vector<AccelerationStructureGeometry>& geometries);

			/**
			 * Destructor.
			 */
			~DX12BottomLevelAccelerationStructure() override;

		private:
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

		private:
			D3D12MA::Allocation* m_pScratchBuffer = nullptr;
			D3D12MA::Allocation* m_pAccelerationStructure = nullptr;
			UINT64 m_ResultDataMaxSizeInBytes = 0;
		};
	}
}