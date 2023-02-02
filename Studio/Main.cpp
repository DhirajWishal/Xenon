// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"
#include "StudioConfiguration.hpp"

void run()
{
	auto studio = Studio(StudioConfiguration::GetInstance().getCurrentBackendType());
	studio.run();
}

int main()
{
	StudioConfiguration::GetInstance().load("StudioConfig.bin");

	while (!StudioConfiguration::GetInstance().shouldExitApplication())
		run();

	StudioConfiguration::GetInstance().save("StudioConfig.bin");

	return 0;
}