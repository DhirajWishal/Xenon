// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"
#include "Layers/ImGuiLayer.hpp"

#include "Xenon/MonoCamera.hpp"
#include "Xenon/Geometry.hpp"
#include "Xenon/FrameTimer.hpp"
#include "Xenon/DefaultCacheHandler.hpp"

#include "XenonCore/Logging.hpp"
#include "XenonBackend/ShaderSource.hpp"

#include "Xenon/Layers/DefaultRasterizingLayer.hpp"
#include "Xenon/Layers/DefaultRayTracingLayer.hpp"
#include "Xenon/Layers/ShadowMapLayer.hpp"
#include "Xenon/Layers/DiffusionLayer.hpp"
#include "Xenon/Layers/GBufferLayer.hpp"

#include "XenonShaderBank/Debugging/Shader.vert.hpp"
#include "XenonShaderBank/Debugging/Shader.frag.hpp"
#include "XenonShaderBank/Billboard/Billboard.vert.hpp"
#include "XenonShaderBank/Billboard/Billboard.frag.hpp"
#include "XenonShaderBank/ShadowMap/Scene.vert.hpp"
#include "XenonShaderBank/ShadowMap/Scene.frag.hpp"

#include "XenonShaderBank/Testing/RayTracing/ClosestHit.rchit.hpp"
#include "XenonShaderBank/Testing/RayTracing/Miss.rmiss.hpp"
#include "XenonShaderBank/Testing/RayTracing/RayGen.rgen.hpp"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

constexpr auto g_DefaultRenderingPriority = 5;

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
	[[nodiscard]] Xenon::Backend::RayTracingPipelineSpecification getRayTracingPipelineSpecification()
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

	// // Create the occlusion layer for occlusion culling.
	// auto pOcclusionLayer = m_Renderer.createLayer<Xenon::OcclusionLayer>(m_Scene.getCamera(), g_DefaultRenderingPriority);
	// pOcclusionLayer->setScene(m_Scene);
	// 
	// // Create the shadow map layer.
	// auto pShadowMapLayer = m_Renderer.createLayer<Xenon::Experimental::ShadowMapLayer>(m_Scene.getCamera());
	// pShadowMapLayer->setScene(m_Scene);

	// Setup the pipeline.
#ifdef XENON_DEV_ENABLE_RAY_TRACING
	auto pRenderTarget = m_Renderer.createLayer<Xenon::DefaultRayTracingLayer>(m_Scene.getCamera());
	pRenderTarget->setScene(m_Scene);

	materialBuidler.setRayTracingPipelineSpecification(getRayTracingPipelineSpecification());
	auto pPipeline = m_Instance.getFactory()->createRayTracingPipeline(m_Instance.getBackendDevice(), std::make_unique<Xenon::DefaultCacheHandler>(), materialBuidler.getRayTracingPipelineSpecification());

	const auto loaderFunction = [this, &pPipeline, &pRenderTarget, &materialBuidler]
	{
		const auto grouping = m_Scene.createGroup();
		[[maybe_unused]] const auto& geometry = m_Scene.create<Xenon::Geometry>(grouping, Xenon::Geometry::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"));
		[[maybe_unused]] const auto& material = m_Scene.create<Xenon::Material>(grouping, materialBuidler);
	};

#else 
	// auto pRenderTarget = m_Renderer.createLayer<Xenon::DefaultRasterizingLayer>(m_Scene.getCamera(), g_DefaultRenderingPriority);
	// pRenderTarget->setScene(m_Scene);
	// pRenderTarget->setOcclusionLayer(pOcclusionLayer);

	Xenon::Backend::RasterizingPipelineSpecification specification;
	// specification.m_VertexShader = Xenon::Generated::CreateShaderShader_vert();
	// specification.m_FragmentShader = Xenon::Generated::CreateShaderShader_frag();
	specification.m_VertexShader = Xenon::Generated::CreateShaderScene_vert();
	specification.m_FragmentShader = Xenon::Generated::CreateShaderScene_frag();
	materialBuidler.setRasterizingPipelineSpecification(specification);

	// materialBuidler.addShadowMap(pShadowMapLayer->getShadowTexture());
	// materialBuidler.addCustomProperty(pShadowMapLayer->getShadowCameraBuffer());

	const auto loaderFunction = [this, &materialBuidler]
	{
		const auto grouping = m_Scene.createGroup();
		[[maybe_unused]] const auto& geometry = m_Scene.create<Xenon::Geometry>(grouping, Xenon::Geometry::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"));
		[[maybe_unused]] const auto& material = m_Scene.create<Xenon::Material>(grouping, materialBuidler);
	};

#endif // XENON_DEV_ENABLE_RAY_TRACING

	// // Create the diffusion layer.
	// auto pDiffusionLayer = m_Renderer.createLayer<Xenon::Experimental::DiffusionLayer>(m_Scene.getCamera()->getWidth(), m_Scene.getCamera()->getHeight(), pRenderTarget->getPriority());
	// pDiffusionLayer->setSourceImage(pRenderTarget->getColorAttachment());

	// Create the layers.
	auto pImGui = m_Renderer.createLayer<ImGuiLayer>(m_Scene.getCamera());
	pImGui->setScene(m_Scene);
	m_Renderer.setScene(m_Scene);

	// Set the layer to be shown.
	// pImGui->showLayer(pRenderTarget);
	pImGui->getLayerView().addLayerOption("GBuffer Positive X Color", m_Renderer.getPositiveXLayer());
	pImGui->getLayerView().addLayerOption("GBuffer Negative X Color", m_Renderer.getNegativeXLayer());
	pImGui->getLayerView().addLayerOption("GBuffer Positive Y Color", m_Renderer.getPositiveYLayer());
	pImGui->getLayerView().addLayerOption("GBuffer Negative Y Color", m_Renderer.getNegativeYLayer());
	pImGui->getLayerView().addLayerOption("GBuffer Positive Z Color", m_Renderer.getPositiveZLayer());
	pImGui->getLayerView().addLayerOption("GBuffer Negative Z Color", m_Renderer.getNegativeZLayer());

	// Create the light source.
	m_LightGroups.emplace_back(createLightSource());

	{
		auto ret = Xenon::XObject::GetJobSystem().insert(loaderFunction);

		Xenon::FrameTimer timer;
		do
		{
			const auto delta = timer.tick();

			// Begin updating the scene.
			m_Scene.beginUpdate();

			// Set the draw call count.
			// pImGui->setDrawCallCount(m_Scene.getDrawableCount(), pRenderTarget->getDrawCount());

			// Begin the ImGui scene.
			// Handle the inputs and update the camera only if we need to.
			if (pImGui->beginFrame(delta))
			{
				updateCamera(delta);
			}

			// Show and update the light sources.
			updateLightSources();

			// End the ImGui scene.
			pImGui->endFrame();

			// End the scene updating process and update the scene object and render everything.
			m_Scene.endUpdate();
		} while (m_Renderer.update());

		m_Scene.cleanup();

		// Wait till the data has been added before quitting.
		ret.get();
	}

	// Cleanup the main objects.
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
}

Xenon::Group Studio::createLightSource()
{
	// Setup the group and add the light source and the quad.
	const auto lighting = m_Scene.createGroup();
	[[maybe_unused]] const auto& lightSource = m_Scene.create<Xenon::Components::LightSource>(lighting, glm::vec4(1.0f), glm::vec3(2.0f), glm::vec3(0.0f), 1.0f, 45.0f);

#ifdef XENON_DEBUG_G
	[[maybe_unused]] const auto& quad = m_Scene.create<Xenon::Geometry>(lighting, Xenon::Geometry::CreateQuad(m_Scene.getInstance()));
	[[maybe_unused]] const auto& transform = m_Scene.create<Xenon::Components::Transform>(lighting, glm::vec3(0), glm::vec3(0), glm::vec3(0.5f));

	// Setup the light bulb image and it's view and sampler.
	auto& bulb = m_Scene.create<LightBulb>(lighting);
	bulb.m_pImage = Xenon::Geometry::CreateImageFromFile(m_Scene.getInstance(), XENON_ASSET_DIR "LightBulb/idea.png");	// TODO: This needs to be standardized.
	bulb.m_pImageView = m_Scene.getInstance().getFactory()->createImageView(m_Scene.getInstance().getBackendDevice(), bulb.m_pImage.get(), {});
	bulb.m_pImageSampler = m_Scene.getInstance().getFactory()->createImageSampler(m_Scene.getInstance().getBackendDevice(), {});

	// Setup the material builder.
	Xenon::MaterialBuilder materialBuidler;
	materialBuidler.addBaseColorTexture({ .m_pImage = bulb.m_pImage.get(), .m_pImageView = bulb.m_pImageView.get(), .m_pImageSampler = bulb.m_pImageSampler.get() });

	// Setup the pipeline specification.
	Xenon::Backend::RasterizingPipelineSpecification specification;
	specification.m_VertexShader = Xenon::Generated::CreateShaderBillboard_vert();
	specification.m_FragmentShader = Xenon::Generated::CreateShaderBillboard_frag();
	specification.m_CullMode = Xenon::Backend::CullMode::None;

	materialBuidler.setRasterizingPipelineSpecification(specification);

	// Create the material.
	[[maybe_unused]] const auto& material = m_Scene.create<Xenon::Material>(lighting, materialBuidler);

#endif // XENON_DEBUG

	return lighting;
}

void Studio::updateLightSources()
{
	ImGui::Begin("Light Sources");
	for (const auto& group : m_LightGroups)
	{
		Xenon::Components::LightSource light = m_Scene.getRegistry().get<Xenon::Components::LightSource>(group);

		ImGui::Text("Light ID: %i", Xenon::EnumToInt(group));
		ImGui::NewLine();
		ImGui::InputFloat3("Position", glm::value_ptr(light.m_Position));
		ImGui::Separator();

		// VS please fix your formatting...
		const auto updateFunction = [light](auto& object) { object = light; };
		m_Scene.getRegistry().patch<Xenon::Components::LightSource>(group, updateFunction);
	}

	ImGui::End();
}
