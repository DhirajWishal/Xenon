// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"
#include "CacheHandler.hpp"

#include "Xenon/MeshStorage.hpp"
#include "Xenon/FrameTimer.hpp"

#include "XenonCore/Logging.hpp"
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
	XENON_LOG_INFORMATION("Starting the {}", GetRendererTitle(m_Instance.getBackendType()));
}

void Studio::run()
{
	m_Renderer.createLayer<Xenon::ClearScreenLayer>(&m_Camera, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	auto pLayer = m_Renderer.createLayer<Xenon::DefaultRasterizingLayer>(&m_Camera);

	auto storage = Xenon::XObject::GetJobSystem().insert([this] { return Xenon::MeshStorage::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"); });

	Xenon::Backend::RasterizingPipelineSpecification specification;
	specification.m_VertexShader = Xenon::Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Debugging/Shader.vert.spv");
	specification.m_FragmentShader = Xenon::Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Debugging/Shader.frag.spv");

	auto pPipeline = m_Instance.getFactory()->createRasterizingPipeline(m_Instance.getBackendDevice(), std::make_unique<CacheHandler>(), pLayer->getRasterizer(), specification);

	bool dataLoaded = false;

	Xenon::FrameTimer timer;
	while (m_Renderer.update())
	{
		const auto delta = timer.tick();

		// Add the draw data when the model has been loaded.
		if (!dataLoaded && is_ready(storage))
		{
			pLayer->addDrawData(storage.get(), pPipeline.get());
			dataLoaded = true;
		}

		// Move the camera.
		switch (m_Renderer.getKeyboard().m_Character)
		{
		case 'w':
		case 'W':
			m_Camera.moveForward(delta);
			break;

		case 'a':
		case 'A':
			m_Camera.moveLeft(delta);
			break;

		case 's':
		case 'S':
			m_Camera.moveDown(delta);
			break;

		case 'd':
		case 'D':
			m_Camera.moveRight(delta);
			break;

		default:
			break;
		}

		m_Camera.update();
	}

	if (!dataLoaded)
		storage.wait();

	m_Instance.cleanup();

	XENON_LOG_INFORMATION("Exiting the {}", GetRendererTitle(m_Instance.getBackendType()));
}
