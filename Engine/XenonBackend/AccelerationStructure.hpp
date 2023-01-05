// Copyright 2022-2023 Dhiraj Wishal
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
	}
}