// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/BottomLevelAccelerationStructure.hpp"

#include "VulkanAccelerationStructure.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan bottom level acceleration structure class.
		 */
		class VulkanBottomLevelAccelerationStructure final : public BottomLevelAccelerationStructure, public VulkanAccelerationStructure
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param geometries The geometries to be stored in the acceleration structure.
			 */
			explicit VulkanBottomLevelAccelerationStructure(VulkanDevice* pDevice, const std::vector<AccelerationStructureGeometry>& geometries);

			/**
			 * Destructor.
			 */
			~VulkanBottomLevelAccelerationStructure() override = default;
		};
	}
}