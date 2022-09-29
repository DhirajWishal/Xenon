// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "UniformBuffer.hpp"

namespace Xenon
{
	UniformBuffer::UniformBuffer(Instance& instance, uint64_t size)
		: m_pUniformBuffer(Globals::BackendFactory->createUniformBuffer(instance.getBackendDevice(), size))
	{
	}
}