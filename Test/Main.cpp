// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "TestContainer.hpp"

#include "InstanceTest/VulkanInstanceTest.hpp"
#include "InstanceTest/DX12InstanceTest.hpp"

int main()
{
	TestContainer container;

	// Instance tests.
	container.insert<VulkanInstanceTest>();
	container.insert<DX12InstanceTest>();

	return static_cast<int>(container.test());
}