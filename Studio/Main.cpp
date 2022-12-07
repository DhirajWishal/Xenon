// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"

void run(Xenon::BackendType backend)
{
	auto studio = Studio(backend);
	studio.run();
}

int main()
{
	// std::vector<std::future<void>> workers;
	// workers.emplace_back(std::async(std::launch::async, run, Xenon::BackendType::Vulkan));
	// workers.emplace_back(std::async(std::launch::async, run, Xenon::BackendType::DirectX_12));

	run(Xenon::BackendType::Vulkan);
	// run(Xenon::BackendType::DirectX_12);

	return 0;
}