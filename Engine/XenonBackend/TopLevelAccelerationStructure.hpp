// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BottomLevelAccelerationStructure.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Top level acceleration structure class.
		 * This structure contains the geometry instances.
		 */
		class TopLevelAccelerationStructure : public AccelerationStructure
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pBottomLevelAccelerationStructures The bottom level acceleration structures.
			 */
			explicit TopLevelAccelerationStructure(const Device* pDevice, [[maybe_unused]] const std::vector<BottomLevelAccelerationStructure*>& pBottomLevelAccelerationStructures) : AccelerationStructure(pDevice) {}
		};
	}
}