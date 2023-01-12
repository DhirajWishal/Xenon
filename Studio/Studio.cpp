// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"
#include "CacheHandler.hpp"
#include "Layers/ImGuiLayer.hpp"

#include "Xenon/MonoCamera.hpp"
#include "Xenon/Geometry.hpp"
#include "Xenon/FrameTimer.hpp"

#include "XenonCore/Logging.hpp"
#include "XenonBackend/ShaderSource.hpp"

#include "Xenon/Layers/DefaultRasterizingLayer.hpp"
#include "Xenon/Layers/DefaultRayTracingLayer.hpp"

#include "XenonShaderBank/Debugging/Shader.vert.hpp"
#include "XenonShaderBank/Debugging/Shader.frag.hpp"

#include "XenonShaderBank/Testing/RayTracing/ClosestHit.rchit.hpp"
#include "XenonShaderBank/Testing/RayTracing/Miss.rmiss.hpp"
#include "XenonShaderBank/Testing/RayTracing/RayGen.rgen.hpp"

#include <imgui.h>

namespace /* anonymous */
{
	/**
	 * Get a suitable renderer title using the backend type.
	 *
	 * @param type The type of the backend.
	 * @return The renderer title.
	 */
	[[nodiscard]] constexpr std::string GetRendererTitle(Xenon::BackendType type)
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

	/**
	 * Check if the future is ready to be read from.
	 *
	 * @tparam R The future's value type.
	 * @param f The future.
	 * @return True if the future is ready to be read from.
	 * @return False if the future is not ready to be read from.
	 */
	template<typename R>
	[[nodiscard]] bool is_ready(std::future<R> const& f)
	{
		return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}

	struct RayPayload final
	{
		glm::vec3 m_Color;
		float m_Distance;
		glm::vec3 m_Normal;
		float m_Reflector;
	};

	struct ProceduralPrimitiveAttributes final
	{
		glm::vec3 m_Normal;
	};

	/**
	 * Create the ray tracing pipeline specification.
	 *
	 * @return The pipeline specification.
	 */
	Xenon::Backend::RayTracingPipelineSpecification getRayTracingPipelineSpecification()
	{
		Xenon::Backend::RayTracingPipelineSpecification specification = {};
		specification.m_ShaderGroups.emplace_back().m_RayGenShader = Xenon::Generated::CreateShaderRayGen_rgen();
		specification.m_ShaderGroups.emplace_back().m_MissShader = Xenon::Generated::CreateShaderMiss_rmiss();
		specification.m_ShaderGroups.emplace_back().m_ClosestHitShader = Xenon::Generated::CreateShaderClosestHit_rchit();

		specification.m_MaxPayloadSize = sizeof(RayPayload);
		specification.m_MaxAttributeSize = sizeof(ProceduralPrimitiveAttributes);

		return specification;
	}
}

Studio::Studio(Xenon::BackendType type /*= Xenon::BackendType::Any*/)
	: m_Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, type)
	, m_Scene(m_Instance, std::make_unique<Xenon::MonoCamera>(m_Instance, 1920, 1080))
	, m_Renderer(m_Instance, m_Scene.getCamera(), GetRendererTitle(type))
{
	XENON_LOG_INFORMATION("Starting the {}", GetRendererTitle(m_Instance.getBackendType()));
}

void Studio::run()
{
	// Setup the main material.
	Xenon::MaterialBuilder materialBuidler;
	materialBuidler.addBaseColorTexture();	// Use the sub mesh's one.

	// Setup the pipeline.
#ifdef XENON_DEV_ENABLE_RAY_TRACING
	auto pRenderTarget = m_Renderer.createLayer<Xenon::DefaultRayTracingLayer>(m_Scene.getCamera());
	pRenderTarget->setScene(m_Scene);

	auto pPipeline = m_Instance.getFactory()->createRayTracingPipeline(m_Instance.getBackendDevice(), std::make_unique<CacheHandler>(), getRayTracingPipelineSpecification());

	const auto loaderFunction = [this, &pPipeline, &pRenderTarget, &materialBuidler]
	{
		const auto grouping = m_Scene.createGroup();
		const auto& geometry = m_Scene.create<Xenon::Geometry>(grouping, Xenon::Geometry::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"));
		const auto& material = m_Scene.create<Xenon::Material>(grouping, materialBuidler.getMaterial());

		pRenderTarget->addDrawData(Xenon::Geometry::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"), pPipeline.get());
};

#else 
	auto pRenderTarget = m_Renderer.createLayer<Xenon::DefaultRasterizingLayer>(m_Scene.getCamera());
	pRenderTarget->setScene(m_Scene);

	Xenon::Backend::RasterizingPipelineSpecification specification;
	specification.m_VertexShader = Xenon::Generated::CreateShaderShader_vert();
	specification.m_FragmentShader = Xenon::Generated::CreateShaderShader_frag();
	auto pPipeline = m_Instance.getFactory()->createRasterizingPipeline(m_Instance.getBackendDevice(), std::make_unique<CacheHandler>(), pRenderTarget->getRasterizer(), specification);

	const auto loaderFunction = [this, &pPipeline, &pRenderTarget, &materialBuidler]
	{
		const auto grouping = m_Scene.createGroup();
		const auto& geometry = m_Scene.create<Xenon::Geometry>(grouping, Xenon::Geometry::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"));
		const auto& material = m_Scene.create<Xenon::Material>(grouping, materialBuidler.getMaterial());

		pRenderTarget->addDrawData(Xenon::Geometry::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"), pPipeline.get());
	};

#endif // XENON_DEV_ENABLE_RAY_TRACING

	// Create the layers.
	auto pImGui = m_Renderer.createLayer<ImGuiLayer>(m_Scene.getCamera());

	// Set the layer to be shown.
	pImGui->showLayer(pRenderTarget);

	{
		auto ret = Xenon::XObject::GetJobSystem().insert(loaderFunction);

		Xenon::FrameTimer timer;
		do
		{
			const auto delta = timer.tick();

			// Set the draw call count.
			pImGui->setDrawCallCount(pRenderTarget->getTotalDrawCount(), pRenderTarget->getDrawCount());

			// Begin the ImGui scene.
			// Handle the inputs and update the camera only if we need to.
			if (pImGui->beginFrame(delta))
				updateCamera(delta);

			// End the ImGui scene.
			pImGui->endFrame();

			// Update the scene object and render everything.
			m_Scene.update();
		} while (m_Renderer.update());

		// Wait till the data has been added before quitting.
		ret.get();
	}

	// Cleanup the renderer and instance.
	m_Renderer.cleanup();
	m_Instance.cleanup();

	XENON_LOG_INFORMATION("Exiting the {}", GetRendererTitle(m_Instance.getBackendType()));
}

void Studio::updateCamera(std::chrono::nanoseconds delta)
{
	// Move the camera.
	if (m_Renderer.getKeyboard().m_KeyW)
		m_Scene.getCamera()->moveForward(delta);

	if (m_Renderer.getKeyboard().m_KeyA)
		m_Scene.getCamera()->moveLeft(delta);

	if (m_Renderer.getKeyboard().m_KeyS)
		m_Scene.getCamera()->moveBackward(delta);

	if (m_Renderer.getKeyboard().m_KeyD)
		m_Scene.getCamera()->moveRight(delta);

	if (m_Renderer.getKeyboard().m_Up)
		m_Scene.getCamera()->moveUp(delta);

	if (m_Renderer.getKeyboard().m_Down)
		m_Scene.getCamera()->moveDown(delta);

	// Rotate the camera.
	if (m_Renderer.getMouse().m_ButtonLeft == Xenon::MouseButtonEvent::Press)
	{
		const auto positionX = m_Renderer.getMouse().m_MousePosition.m_XAxis * -1.0f;
		const auto positionY = m_Renderer.getMouse().m_MousePosition.m_YAxis * -1.0f;

		if (m_bFirstMouse)
		{
			m_LastX = positionX;
			m_LastY = positionY;
			m_bFirstMouse = false;
		}

		const float xoffset = (positionX - m_LastX) * m_Scene.getCamera()->m_RotationBias * 0.75f;
		const float yoffset = (m_LastY - positionY) * m_Scene.getCamera()->m_RotationBias; // Reversed since y-coordinates go from bottom to top

		m_LastX = positionX;
		m_LastY = positionY;

		m_Scene.getCamera()->updateYaw(xoffset, delta);
		m_Scene.getCamera()->updatePitch(yoffset, delta);
	}
	else
	{
		m_bFirstMouse = true;
	}

	m_Scene.getCamera()->update();
}
