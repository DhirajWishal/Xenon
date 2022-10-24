// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "XenonCore/Logging.hpp"
#include "Xenon/Instance.hpp"
#include "Xenon/VertexSpecification.hpp"
#include "Xenon/MeshStorage.hpp"

void run(Xenon::BackendType backend)
{
	auto instance = Xenon::Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, backend);
	auto storage = Xenon::Instance::GetJobSystem().insert([&instance] { return Xenon::MeshStorage::FromFile(instance, "E:\\Flint\\ThirdParty\\glTF-Sample-Models\\2.0\\Sponza\\glTF\\Sponza.gltf"); });

	Xenon::Instance::GetJobSystem().wait();
	storage.wait();
}

int main()
{
	XENON_LOG_INFORMATION("Hello from the Xenon Studio!");

	// Run using Direct X 12.
	XENON_LOG_INFORMATION("Running Xenon Studio using the DirectX 12 backend.");
	run(Xenon::BackendType::DirectX_12);

	// Run using Vulkan.
	XENON_LOG_INFORMATION("Running Xenon Studio using the Vulkan backend.");
	run(Xenon::BackendType::Vulkan);

	return 0;
}