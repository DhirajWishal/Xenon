// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/TopLevelAccelerationStructure.hpp"

#include "DX12AccelerationStructure.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 top level acceleration structure class.
		 */
		class DX12TopLevelAccelerationStructure final : public TopLevelAccelerationStructure, public DX12AccelerationStructure
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pBottomLevelAccelerationStructures The bottom level acceleration structures.
			 */
			explicit DX12TopLevelAccelerationStructure(DX12Device* pDevice, const std::vector<BottomLevelAccelerationStructure*>& pBottomLevelAccelerationStructures);

			/**
			 * Destructor.
			 */
			~DX12TopLevelAccelerationStructure() override = default;
		};
	}
}