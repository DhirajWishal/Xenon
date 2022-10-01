// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "XenonCore/Logging.hpp"
#include "Xenon/VertexBuffer.hpp"
#include "Xenon/StagingBuffer.hpp"

int main()
{
	XENON_LOG_INFORMATION("Hello from the Xenon Studio!");
	auto instance = Xenon::Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, Xenon::BackendType::DirectX_12);
	// auto instance = Xenon::Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, Xenon::BackendType::Vulkan);
	auto vertexBuffer = Xenon::VertexBuffer(instance, 1024, 3 * sizeof(float));
	auto stagingBuffer = Xenon::StagingBuffer(instance, 1024);

	return 0;
}