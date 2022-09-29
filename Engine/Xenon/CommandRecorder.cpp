// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "CommandRecorder.hpp"

namespace Xenon
{
	CommandRecorder::CommandRecorder(Instance& instance, uint32_t bufferCount)
		: m_pCommandBuffers(Globals::BackendFactory->createCommandBuffers(instance.getBackendDevice(), bufferCount))
	{
	}

	CommandRecorder::~CommandRecorder()
	{
	}
}