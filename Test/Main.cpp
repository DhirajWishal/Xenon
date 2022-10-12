// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "TestContainer.hpp"

#include "InstanceTests/VulkanInstanceTest.hpp"
#include "InstanceTests/DX12InstanceTest.hpp"

int main()
{
	TestContainer container;

	// Instance tests.
	container.insert<VulkanInstanceTest>();
	container.insert<DX12InstanceTest>();

	// Execute the tests!
	return static_cast<int>(container.test());
}