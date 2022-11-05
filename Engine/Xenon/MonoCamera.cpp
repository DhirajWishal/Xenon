// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MonoCamera.hpp"

namespace Xenon
{
	MonoCamera::MonoCamera(Instance& instance, uint32_t width, uint32_t height)
		: Camera(width, height)
	{
		// Create the uniform buffer.
		m_pUniformBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), sizeof(CameraBuffer), Backend::BufferType::Uniform);

		// Setup the viewport.
		m_Viewport.m_pUniformBuffer = m_pUniformBuffer.get();
		m_Viewport.m_Width = static_cast<float>(width);
		m_Viewport.m_Height = static_cast<float>(height);

		// Initialize the old time point.
		m_OldTimePoint = Clock::now();
	}

	std::chrono::nanoseconds MonoCamera::update()
	{
		const auto currentTimePoint = Clock::now();
		std::chrono::nanoseconds delta;

		// Cast the time point if required.
		if constexpr (std::is_same_v<Clock::duration, std::chrono::nanoseconds>)
			delta = currentTimePoint - m_OldTimePoint;

		else
			delta = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimePoint - m_OldTimePoint);

		m_OldTimePoint = currentTimePoint;
		return delta;
	}
}