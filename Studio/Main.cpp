// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "XenonCore/Logging.hpp"
#include "Xenon/Instance.hpp"
#include "Xenon/VertexSpecification.hpp"
#include "Xenon/MeshStorage.hpp"

#include "XenonPlatform/IFactory.hpp"

void run(Xenon::BackendType backend)
{
	if (backend == Xenon::BackendType::DirectX_12)
		XENON_LOG_INFORMATION("Running Xenon Studio using the DirectX 12 backend.");

	else if (backend == Xenon::BackendType::Vulkan)
		XENON_LOG_INFORMATION("Running Xenon Studio using the Vulkan backend.");

	auto instance = Xenon::Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, backend);
	auto storage = Xenon::MeshStorage::FromFile(instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf");
}

int main()
{
	XENON_LOG_INFORMATION("Hello from the Xenon Studio!");

	auto factory = Xenon::Platform::IFactory::Create();
	auto window = factory->createWindow("Xenon Studio", 1280, 720);

	std::vector<std::jthread> backends;
	backends.emplace_back(run, Xenon::BackendType::DirectX_12);
	backends.emplace_back(run, Xenon::BackendType::Vulkan);

	return 0;
}