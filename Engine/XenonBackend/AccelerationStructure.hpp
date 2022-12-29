// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Acceleration structure class.
		 * This structure is used to store geometry and instance information (bottom level and top level) which is used for ray tracing.
		 */
		class AccelerationStructure : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit AccelerationStructure([[maybe_unused]] const Device* pDevice) {}
		};

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
			 */
			explicit TopLevelAccelerationStructure(const Device* pDevice) : AccelerationStructure(pDevice) {}
		};
	}
}