// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"
#include "CacheHandler.hpp"

#include "XenonCore/Logging.hpp"
#include "Xenon/MeshStorage.hpp"
#include "XenonBackend/ShaderSource.hpp"

#include "Xenon/Layers/ClearScreenLayer.hpp"
#include "Xenon/Layers/DefaultRasterizingLayer.hpp"

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

	auto pUniformBuffer = m_Instance.getFactory()->createBuffer(m_Instance.getBackendDevice(), sizeof(float[4][4]), Xenon::Backend::BufferType::Uniform);

	auto pDescriptor = m_Instance.getFactory()->createDescriptor(m_Instance.getBackendDevice(), bindingInfo, Xenon::Backend::DescriptorType::UserDefined);
	pDescriptor->attach(0, pUniformBuffer.get());
}

void Studio::run()
{
	m_Renderer.createLayer<Xenon::ClearScreenLayer>(m_Instance, &m_Camera, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	auto pLayer = m_Renderer.createLayer<Xenon::DefaultRasterizingLayer>(m_Instance, &m_Camera);

	auto storage = Xenon::XObject::GetJobSystem().insert([this] { return Xenon::MeshStorage::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"); });

	Xenon::Backend::RasterizingPipelineSpecification specification;
	specification.m_VertexShader = Xenon::Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Debugging/Shader.vert.spv");
	specification.m_FragmentShader = Xenon::Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Debugging/Shader.frag.spv");

	auto pPipeline = m_Instance.getFactory()->createRasterizingPipeline(m_Instance.getBackendDevice(), std::make_unique<CacheHandler>(), pLayer->getRasterizer(), specification);

	bool dataLoaded = false;
	while (m_Renderer.update())
	{
		if (!dataLoaded && is_ready(storage))
		{
			pLayer->addDrawData(storage.get(), pPipeline.get());
			dataLoaded = true;
		}
	}

	XENON_LOG_INFORMATION("Exiting the {}", GetRendererTitle(m_Instance.getBackendType()));
}
