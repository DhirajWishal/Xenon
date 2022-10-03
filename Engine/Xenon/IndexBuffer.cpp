// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "IndexBuffer.hpp"

namespace Xenon
{
	IndexBuffer::IndexBuffer(Instance& instance, uint64_t size, IndexSize indexSize)
		: m_pIndexBuffer(instance.getFactory()->createIndexBuffer(instance.getBackendDevice(), size, indexSize))
	{
	}
}