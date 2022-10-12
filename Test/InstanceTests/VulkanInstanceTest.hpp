// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Test.hpp"

/**
 * Vulkan instance test class.
 */
class VulkanInstanceTest final : public Test
{
public:
	VulkanInstanceTest() : Test("Vulkan Instance Test") {}
	~VulkanInstanceTest() override = default;

	[[nodiscard]] int32_t onExecute() override;
};