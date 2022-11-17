// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"

#include "XenonCore/Logging.hpp"

void run(Xenon::BackendType backend)
{
	auto studio = Studio(backend);
	studio.run();
}

int main()
{
	XENON_LOG_INFORMATION("Hello from the Xenon Studio!");

	std::vector<std::jthread> backends;
	backends.emplace_back(run, Xenon::BackendType::DirectX_12);
	backends.emplace_back(run, Xenon::BackendType::Vulkan);

	return 0;
}