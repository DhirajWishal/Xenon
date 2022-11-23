// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"

#include "XenonCore/Logging.hpp"
#include "Xenon/Layers/ClearScreenLayer.hpp"
#include "Xenon/MeshStorage.hpp"
#include "XenonBackend/ShaderSource.hpp"

namespace /* anonymous */
{
	/**
	 * Get a suitable renderer title using the backend type.
	 *
	 * @param type The type of the backend.
	 * @return The renderer title.
	 */
	std::string GetRendererTitle(Xenon::BackendType type)
	{
		switch (type)
		{
		case Xenon::BackendType::Vulkan:
			return "Xenon Studio - Vulkan";

		case Xenon::BackendType::DirectX_12:
			return "Xenon Studio - DirectX 12";

		case Xenon::BackendType::Any:
			return "Xenon Studio - Any (automatic)";

		default:
			return "Xenon Studio";
		}
	}

	template<typename R>
	bool is_ready(std::future<R> const& f)
	{
		return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}
}

Studio::Studio(Xenon::BackendType type /*= Xenon::BackendType::Any*/)
	: m_Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, type)
	, m_Camera(m_Instance, 1280, 720)
	, m_Renderer(m_Instance, &m_Camera, GetRendererTitle(type))
{

	std::vector<Xenon::Backend::DescriptorBindingInfo> bindingInfo;
	auto& info = bindingInfo.emplace_back();
	info.m_ApplicableShaders = Xenon::Backend::ShaderType::Vertex;
	info.m_Type = Xenon::Backend::ResourceType::UniformBuffer;

	auto pDescriptor = m_Instance.getFactory()->createDescriptor(m_Instance.getBackendDevice(), bindingInfo, Xenon::Backend::DescriptorType::UserDefined);

	XENON_TODO_NOW("(Dhiraj) Testing TODO statement.");
}

void Studio::run()
{
	m_Renderer.createLayer<Xenon::ClearScreenLayer>(m_Instance, &m_Camera, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	auto storage = Xenon::XObject::GetJobSystem().insert([this] { return Xenon::MeshStorage::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"); });
	auto shader = Xenon::XObject::GetJobSystem().insert([] { return Xenon::Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Debugging/Shader.vert.spv"); });

	bool shaderHandled = false;
	while (m_Renderer.update())
	{
		if (!shaderHandled && is_ready(shader))
		{
			auto shaderSource = shader.get();
			shaderSource.performReflection();

			shaderHandled = true;
		}
	}

	XENON_LOG_INFORMATION("Exiting the {}", GetRendererTitle(m_Instance.getBackendType()));
	[[maybe_unused]] auto meshStorage = storage.get();
}
