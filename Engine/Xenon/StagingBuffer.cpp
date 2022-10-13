// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "StagingBuffer.hpp"
#include "../XenonCore/Logging.hpp"

namespace Xenon
{
	StagingBuffer::StagingBuffer(Instance& instance, uint64_t size)
		: m_pStagingBuffer(instance.getFactory()->createStagingBuffer(instance.getBackendDevice(), size))
	{
		int var = 0;
		m_pStagingBuffer->write(reinterpret_cast<std::byte*>(&var), sizeof(var), 0);
		auto view = m_pStagingBuffer->read();
		HexDump(view.begin(), view.end());
	}
}