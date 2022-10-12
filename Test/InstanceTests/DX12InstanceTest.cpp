// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12InstanceTest.hpp"
#include "Xenon/Instance.hpp"

int32_t DX12InstanceTest::onExecute()
{
	auto instance = Xenon::Instance("Xenon Test", 1, Xenon::RenderTargetType::All, Xenon::BackendType::DirectX_12);
	return 0;
}
