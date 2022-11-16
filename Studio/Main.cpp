// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "XenonCore/Logging.hpp"
#include "Xenon/Instance.hpp"
#include "Xenon/VertexSpecification.hpp"
#include "Xenon/MeshStorage.hpp"
#include "Xenon/Renderer.hpp"
#include "Xenon/MonoCamera.hpp"

#include "Xenon/Layers/ClearScreenLayer.hpp"

// https://stackoverflow.com/a/10917945/11228029
template<typename R>
bool is_ready(std::future<R> const& f)
{
	return f.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready;
}

void run(Xenon::BackendType backend)
{
	if (backend == Xenon::BackendType::DirectX_12)
		XENON_LOG_INFORMATION("Running Xenon Studio using the DirectX 12 backend.");

	else if (backend == Xenon::BackendType::Vulkan)
		XENON_LOG_INFORMATION("Running Xenon Studio using the Vulkan backend.");

	auto instance = Xenon::Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, backend);
	auto camera = Xenon::MonoCamera(instance, 1280, 720);
	auto renderer = Xenon::Renderer(instance, &camera, backend == Xenon::BackendType::Vulkan ? "Xenon Studio - Vulkan" : "Xenon Studio - DirectX 12");
	renderer.createLayer<Xenon::ClearScreenLayer>(instance, &camera, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	auto storage = Xenon::XObject::GetJobSystem().insert([&instance] { return Xenon::MeshStorage::FromFile(instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"); });
	while (renderer.update());

	[[maybe_unused]] auto meshStorage = storage.get();
}

int main()
{
	XENON_LOG_INFORMATION("Hello from the Xenon Studio!");

	std::vector<std::jthread> backends;
	backends.emplace_back(run, Xenon::BackendType::DirectX_12);
	backends.emplace_back(run, Xenon::BackendType::Vulkan);

	while (true)
	{
		bool isJoinable = false;
		for (const auto& backend : backends)
		{
			if (backend.joinable())
			{
				if (!isJoinable)
					isJoinable = true;

				else
					return 0;
			}
		}
	}

	return 0;
}