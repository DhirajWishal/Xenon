// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "XenonCore/Logging.hpp"
#include "Xenon/VertexBuffer.hpp"
#include "Xenon/StagingBuffer.hpp"
#include "Xenon/VertexTraits.hpp"

int main()
{
	XENON_LOG_INFORMATION("Hello from the Xenon Studio!");
	auto instance = Xenon::Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, Xenon::BackendType::DirectX_12);
	// auto instance = Xenon::Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, Xenon::BackendType::Vulkan);
	auto vertexBuffer = Xenon::VertexBuffer(instance, 1024, 3 * sizeof(float));
	auto stagingBuffer = Xenon::StagingBuffer(instance, 1024);

	Xenon::VertexSpecification specification;
	specification.addElement(Xenon::VertexElement::Position, sizeof(float));
	specification.addElement(Xenon::VertexElement::Normal, sizeof(float));
	specification.addElement(Xenon::VertexElement::Color_0, sizeof(uint8_t));
	specification.addElement(Xenon::VertexElement::TextureCoordinate_0);

	return 0;
}