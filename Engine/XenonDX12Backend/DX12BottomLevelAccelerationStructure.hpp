// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/BottomLevelAccelerationStructure.hpp"

#include "DX12AccelerationStructure.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 bottom level acceleration structure class.
		 */
		class DX12BottomLevelAccelerationStructure final : public BottomLevelAccelerationStructure, public DX12AccelerationStructure
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
			~DX12BottomLevelAccelerationStructure() override = default;
		};
	}
}