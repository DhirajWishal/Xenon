// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"
#include "Globals.hpp"

void run(Xenon::BackendType backend)
{
	g_Globals.m_CurrentBackendType = backend;

	auto studio = Studio(backend);
	studio.run();
}

int main()
{
	// g_Globals.m_RequiredBackendType = Xenon::BackendType::Vulkan;
	g_Globals.m_RequiredBackendType = Xenon::BackendType::DirectX_12;
	while (!g_Globals.m_bExitAppliation)
		run(g_Globals.m_RequiredBackendType);

	return 0;
}