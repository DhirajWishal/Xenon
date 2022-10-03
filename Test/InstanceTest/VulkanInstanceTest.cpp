// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanInstanceTest.hpp"
#include "Xenon/Instance.hpp"

int32_t VulkanInstanceTest::onExecute()
{
	auto instance = Xenon::Instance("Xenon Test", 1, Xenon::RenderTargetType::All, Xenon::BackendType::Vulkan);
	return 0;
}
