// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Xenon.hpp"

namespace Xenon
{
	Application::Application(const std::string& name, uint64_t version)
		: m_Name(name), m_Version(version)
	{
	}

	void Application::execute()
	{
		auto timePoint = std::chrono::system_clock::now();
		while (m_bShouldRun)
		{
			const auto currentTimePoint = std::chrono::system_clock::now();
			onUpdate(std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint - timePoint));

			timePoint = currentTimePoint;
		}
	}
}