// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Descriptor.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Descriptor::DX12Descriptor(DX12Device* pDevice, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType type)
			: Descriptor(pDevice, bindingInfo, type)
			, DX12DeviceBoundObject(pDevice)
		{

		}
	}
}