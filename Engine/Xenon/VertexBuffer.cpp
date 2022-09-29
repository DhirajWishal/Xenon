// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VertexBuffer.hpp"

namespace Xenon
{
	VertexBuffer::VertexBuffer(Instance& instance, uint64_t size, uint64_t stride)
		: m_pVertexBuffer(Globals::BackendFactory->createVertexBuffer(instance.getBackendDevice(), size, stride))
	{
	}
}