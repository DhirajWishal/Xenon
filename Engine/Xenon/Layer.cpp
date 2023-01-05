// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Layer.hpp"
#include "Renderer.hpp"

namespace Xenon
{
	Layer::Layer(Renderer& renderer)
		: m_Renderer(renderer)
		, m_pCommandRecorder(renderer.getInstance().getFactory()->createCommandRecorder(renderer.getInstance().getBackendDevice(), Backend::CommandRecorderUsage::Graphics, renderer.getCommandRecorder()->getBufferCount()))
	{
	}
}