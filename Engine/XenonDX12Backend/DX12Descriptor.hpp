// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Descriptor.hpp"

#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 descriptor class.
		 */
		class DX12Descriptor final : public Descriptor, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param bindingInfo The descriptor's binding information. Make sure that the binding information are in the binding order (the first one is binging 0, second is 1 and so on).
			 * @param type The descriptor type.
			 */
			explicit DX12Descriptor(DX12Device* pDevice, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType type);
		};
	}
}