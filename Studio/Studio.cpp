// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"
#include "CacheHandler.hpp"
#include "Layers/ImGuiLayer.hpp"

#include "Xenon/MeshStorage.hpp"
#include "Xenon/FrameTimer.hpp"

#include "XenonCore/Logging.hpp"
#include "XenonBackend/ShaderSource.hpp"

#include "Xenon/Layers/DefaultRasterizingLayer.hpp"

#include "XenonShaderBuilder/VertexShader.hpp"

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
}

Studio::Studio(Xenon::BackendType type /*= Xenon::BackendType::Any*/)
	: m_Instance("Xenon Studio", 0, Xenon::RenderTargetType::All, type)
	, m_Camera(m_Instance, 1920, 1080)
	, m_Renderer(m_Instance, &m_Camera, GetRendererTitle(type))
{
	XENON_LOG_INFORMATION("Starting the {}", GetRendererTitle(m_Instance.getBackendType()));
}

void Studio::run()
{
	// Create the layers.
	auto pLayer = m_Renderer.createLayer<Xenon::DefaultRasterizingLayer>(&m_Camera);
	auto pImGui = m_Renderer.createLayer<ImGuiLayer>(&m_Camera);

	// Set the layer to be shown.
	pImGui->showLayer(pLayer);

	// Setup the pipeline.
	Xenon::Backend::RasterizingPipelineSpecification specification;
	specification.m_VertexShader = Xenon::Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Debugging/Shader.vert.spv");
	specification.m_FragmentShader = Xenon::Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Debugging/Shader.frag.spv");
	auto pPipeline = m_Instance.getFactory()->createRasterizingPipeline(m_Instance.getBackendDevice(), std::make_unique<CacheHandler>(), pLayer->getRasterizer(), specification);

	{
		auto ret = Xenon::XObject::GetJobSystem().insert([this, &pPipeline, &pLayer]
			{
				pLayer->addDrawData(Xenon::MeshStorage::FromFile(m_Instance, XENON_GLTF_ASSET_DIR "2.0/Sponza/glTF/Sponza.gltf"), pPipeline.get());
			}
		);

		Xenon::FrameTimer timer;
		do
		{
			const auto delta = timer.tick();

			// Set the draw call count.
			pImGui->setDrawCallCount(pLayer->getTotalDrawCount(), pLayer->getDrawCount());

			// Begin the ImGui scene.
			// Handle the inputs and update the camera only if we need to.
			if (pImGui->beginFrame(delta))
				updateCamera(delta);

			// End the ImGui scene and render everything.
			pImGui->endFrame();
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
		m_Camera.moveForward(delta);

	if (m_Renderer.getKeyboard().m_KeyA)
		m_Camera.moveLeft(delta);

	if (m_Renderer.getKeyboard().m_KeyS)
		m_Camera.moveBackward(delta);

	if (m_Renderer.getKeyboard().m_KeyD)
		m_Camera.moveRight(delta);

	if (m_Renderer.getKeyboard().m_Up)
		m_Camera.moveUp(delta);

	if (m_Renderer.getKeyboard().m_Down)
		m_Camera.moveDown(delta);

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

		const float xoffset = (positionX - m_LastX) * m_Camera.m_RotationBias * 0.75f;
		const float yoffset = (m_LastY - positionY) * m_Camera.m_RotationBias; // Reversed since y-coordinates go from bottom to top

		m_LastX = positionX;
		m_LastY = positionY;

		m_Camera.updateYaw(xoffset, delta);
		m_Camera.updatePitch(yoffset, delta);
	}
	else
		m_bFirstMouse = true;

	m_Camera.update();
}
