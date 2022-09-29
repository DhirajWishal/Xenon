// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "StagingBuffer.hpp"

namespace Xenon
{
	StagingBuffer::StagingBuffer(Instance& instance, uint64_t size)
		: m_pStagingBuffer(Globals::BackendFactory->createStagingBuffer(instance.getBackendDevice(), size))
	{
	}
}