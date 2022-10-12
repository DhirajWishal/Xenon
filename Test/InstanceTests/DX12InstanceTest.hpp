// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Test.hpp"

/**
 * DirectX 12 instance test class.
 */
class DX12InstanceTest final : public Test
{
public:
	DX12InstanceTest() : Test("DirectX 12 Instance Test") {}
	~DX12InstanceTest() override = default;

	[[nodiscard]] int32_t onExecute() override;
};