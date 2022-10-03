// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "StorageBuffer.hpp"

namespace Xenon
{
	StorageBuffer::StorageBuffer(Instance& instance, uint64_t size)
		: m_pStorageBuffer(instance.getFactory()->createStorageBuffer(instance.getBackendDevice(), size))
	{
	}
}