// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/TopLevelAccelerationStructure.hpp"

#include "VulkanAccelerationStructure.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan top level acceleration structure.
		 */
		class VulkanTopLevelAccelerationStructure final : public TopLevelAccelerationStructure, public VulkanAccelerationStructure
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pBottomLevelAccelerationStructures The bottom level acceleration structures.
			 */
			explicit VulkanTopLevelAccelerationStructure(VulkanDevice* pDevice, const std::vector<BottomLevelAccelerationStructure*>& pBottomLevelAccelerationStructures);

			/**
			 * Destructor.
			 */
			~VulkanTopLevelAccelerationStructure() override = default;
		};
	}
}