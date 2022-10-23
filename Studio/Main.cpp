// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "XenonCore/Logging.hpp"
#include "Xenon/Instance.hpp"
#include "Xenon/VertexSpecification.hpp"
#include "Xenon/MeshStorage.hpp"

void run(Xenon::BackendType backend)
{
	auto instance = Xenon::Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, backend);

	Xenon::VertexSpecification specification;
	specification.addElement(Xenon::VertexElement::Position);
	specification.addElement(Xenon::VertexElement::Normal);
	specification.addElement(Xenon::VertexElement::Color_0, sizeof(uint8_t));
	specification.addElement(Xenon::VertexElement::TextureCoordinate_0);

	const auto storage = Xenon::MeshStorage::FromFile(instance, "E:\\Flint\\ThirdParty\\glTF-Sample-Models\\2.0\\Cube\\glTF\\Cube.gltf");
}

int main()
{
	XENON_LOG_INFORMATION("Hello from the Xenon Studio!");

	// Run using Direct X.
	XENON_LOG_INFORMATION("Running Xenon Studio using the DirectX 12 backend.");
	run(Xenon::BackendType::DirectX_12);

	// Run using Vulkan.
	XENON_LOG_INFORMATION("Running Xenon Studio using the Vulkan backend.");
	run(Xenon::BackendType::Vulkan);

	return 0;
}