// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "StagingBuffer.hpp"

namespace Xenon
{
	StagingBuffer::StagingBuffer(Instance& instance, uint64_t size)
		: m_pStagingBuffer(Globals::BackendFactory->createStagingBuffer(instance.getBackendDevice(), size))
	{
		int var = 0;
		m_pStagingBuffer->write(reinterpret_cast<std::byte*>(&var), sizeof(var), 0);
		const auto ptr = m_pStagingBuffer->beginRead();
		m_pStagingBuffer->endRead();
	}
}