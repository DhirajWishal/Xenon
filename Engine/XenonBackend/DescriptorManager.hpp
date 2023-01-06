// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"
#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Descriptor manager class.
		 * This class is used to create and manage a single type of descriptors.
		 */
		class DescriptorManager : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit DescriptorManager([[maybe_unused]] const Device* pDevice) {}
		};
	}
}