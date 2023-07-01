// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGuiLayer.hpp"

#include "../StudioConfiguration.hpp"
#include "../Logging.hpp"

#include "../Shaders/ImGuiLayer/ImGuiLayer.vert.hpp"
#include "../Shaders/ImGuiLayer/ImGuiLayer.frag.hpp"

#include "Xenon/DefaultCacheHandler.hpp"
#include "Xenon/Renderer.hpp"
#include "Xenon/MonoCamera.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

constexpr auto g_DefaultMaterialHash = 0;

namespace /* anonymous */
{
	/**
	 * Create a color from the float value from 0 - 255.
	 *
	 * @param value The color value.
	 * @return The created color value.
	 */
	XENON_NODISCARD constexpr float CreateColor256(float value) noexcept { return value / 256; }
}

ImGuiLayer::ImGuiLayer(Xenon::Renderer& renderer, uint32_t width, uint32_t height)
	: RasterizingLayer(renderer, 100, width, height, Xenon::Backend::AttachmentType::Color)
	, m_UIStorage(this)
	, m_pVertexBuffers(renderer.getCommandRecorder()->getBufferCount())
	, m_pIndexBuffers(renderer.getCommandRecorder()->getBufferCount())
	, m_ClearValues({ glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) })
{
	ImGui::CreateContext();

	configureImGui();
	setupPipeline();
	setupDefaultMaterial();

	// Setup the ImGui logger.
	auto logger = std::make_shared<spdlog::logger>(g_XenonLoggerName, m_UIStorage.m_pLogs);
	spdlog::register_logger(logger);

	// Setup the pop-ups.
	m_PopUpList.emplace_back(PopUp(g_WelcomePopUpID, "Welcome to the Xenon Studio!", { PopUpOption("Ok", [this] { ImGui::CloseCurrentPopup(); m_bShouldShowGreetingPopUp = false; }) }, PopUpType::Overlay));
	m_PopUpList.emplace_back(PopUp(g_LoadingAssetsPopUpID, "Please wait while the asset is loaded to the engine.", { PopUpOption("Ok", [this] { ImGui::CloseCurrentPopup(); }) }, PopUpType::PopUp));
}

ImGuiLayer::~ImGuiLayer()
{
	// Remove the ImGui logger.
	spdlog::drop(g_XenonLoggerName);

	ImGui::DestroyContext();
}

bool ImGuiLayer::beginFrame(std::chrono::nanoseconds delta)
{
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	// Process the ImGui events.
	auto& io = ImGui::GetIO();

	// Set the display size in case it was resized.
	io.DisplaySize.x = static_cast<float>(m_Renderer.getWindow()->getWidth());
	io.DisplaySize.y = static_cast<float>(m_Renderer.getWindow()->getHeight());

	// Set the time difference.
	io.DeltaTime = static_cast<float>(delta.count()) / std::nano::den;

	// Add the mouse position.
	io.AddMousePosEvent(m_Renderer.getMouse().m_MousePosition.m_XAxis, m_Renderer.getMouse().m_MousePosition.m_YAxis);

	// Add the mouse wheel events.
	io.AddMouseWheelEvent(m_Renderer.getMouse().m_HScroll, m_Renderer.getMouse().m_VScroll);

	// Add the mouse button presses.
	io.AddMouseButtonEvent(ImGuiMouseButton_Left, m_Renderer.getMouse().m_ButtonLeft == Xenon::MouseButtonEvent::Press || m_Renderer.getMouse().m_ButtonLeft == Xenon::MouseButtonEvent::DoublePress);
	io.AddMouseButtonEvent(ImGuiMouseButton_Middle, m_Renderer.getMouse().m_ButtonMiddle == Xenon::MouseButtonEvent::Press || m_Renderer.getMouse().m_ButtonMiddle == Xenon::MouseButtonEvent::DoublePress);
	io.AddMouseButtonEvent(ImGuiMouseButton_Right, m_Renderer.getMouse().m_ButtonRight == Xenon::MouseButtonEvent::Press || m_Renderer.getMouse().m_ButtonRight == Xenon::MouseButtonEvent::DoublePress);

	// Add the input character.
	io.AddInputCharacter(m_Renderer.getKeyboard().m_Character);

	// Add the key events.
	io.AddKeyEvent(ImGuiKey_Space, m_Renderer.getKeyboard().m_Space);

	io.AddKeyEvent(ImGuiKey_Apostrophe, m_Renderer.getKeyboard().m_Apostrophe);
	io.AddKeyEvent(ImGuiKey_Comma, m_Renderer.getKeyboard().m_Comma);
	io.AddKeyEvent(ImGuiKey_Minus, m_Renderer.getKeyboard().m_Minus);
	io.AddKeyEvent(ImGuiKey_Period, m_Renderer.getKeyboard().m_Period);
	io.AddKeyEvent(ImGuiKey_Slash, m_Renderer.getKeyboard().m_Slash);

	io.AddKeyEvent(ImGuiKey_0, m_Renderer.getKeyboard().m_KeyZero);
	io.AddKeyEvent(ImGuiKey_1, m_Renderer.getKeyboard().m_KeyOne);
	io.AddKeyEvent(ImGuiKey_2, m_Renderer.getKeyboard().m_KeyTwo);
	io.AddKeyEvent(ImGuiKey_3, m_Renderer.getKeyboard().m_KeyThree);
	io.AddKeyEvent(ImGuiKey_4, m_Renderer.getKeyboard().m_KeyFour);
	io.AddKeyEvent(ImGuiKey_5, m_Renderer.getKeyboard().m_KeyFive);
	io.AddKeyEvent(ImGuiKey_6, m_Renderer.getKeyboard().m_KeySix);
	io.AddKeyEvent(ImGuiKey_7, m_Renderer.getKeyboard().m_KeySeven);
	io.AddKeyEvent(ImGuiKey_8, m_Renderer.getKeyboard().m_KeyEight);
	io.AddKeyEvent(ImGuiKey_9, m_Renderer.getKeyboard().m_KeyNine);

	io.AddKeyEvent(ImGuiKey_Semicolon, m_Renderer.getKeyboard().m_Semicolon);
	io.AddKeyEvent(ImGuiKey_Equal, m_Renderer.getKeyboard().m_Equal);
	io.AddKeyEvent(ImGuiKey_LeftBracket, m_Renderer.getKeyboard().m_LeftBracket);
	io.AddKeyEvent(ImGuiKey_RightBracket, m_Renderer.getKeyboard().m_RightBracket);
	io.AddKeyEvent(ImGuiKey_Backslash, m_Renderer.getKeyboard().m_Backslash);
	io.AddKeyEvent(ImGuiKey_GraveAccent, m_Renderer.getKeyboard().m_GraveAccent);
	io.AddKeyEvent(ImGuiKey_Escape, m_Renderer.getKeyboard().m_Escape);
	io.AddKeyEvent(ImGuiKey_Enter, m_Renderer.getKeyboard().m_Enter);
	io.AddKeyEvent(ImGuiKey_Tab, m_Renderer.getKeyboard().m_Tab);
	io.AddKeyEvent(ImGuiKey_Backspace, m_Renderer.getKeyboard().m_Backspace);
	io.AddKeyEvent(ImGuiKey_Insert, m_Renderer.getKeyboard().m_Insert);
	io.AddKeyEvent(ImGuiKey_Delete, m_Renderer.getKeyboard().m_Delete);

	io.AddKeyEvent(ImGuiKey_RightArrow, m_Renderer.getKeyboard().m_Right);
	io.AddKeyEvent(ImGuiKey_LeftArrow, m_Renderer.getKeyboard().m_Left);
	io.AddKeyEvent(ImGuiKey_DownArrow, m_Renderer.getKeyboard().m_Down);
	io.AddKeyEvent(ImGuiKey_UpArrow, m_Renderer.getKeyboard().m_Up);

	io.AddKeyEvent(ImGuiKey_PageUp, m_Renderer.getKeyboard().m_PageUp);
	io.AddKeyEvent(ImGuiKey_PageDown, m_Renderer.getKeyboard().m_PageDown);

	io.AddKeyEvent(ImGuiKey_Home, m_Renderer.getKeyboard().m_Home);
	io.AddKeyEvent(ImGuiKey_End, m_Renderer.getKeyboard().m_End);
	io.AddKeyEvent(ImGuiKey_CapsLock, m_Renderer.getKeyboard().m_CapsLock);
	io.AddKeyEvent(ImGuiKey_ScrollLock, m_Renderer.getKeyboard().m_ScrollLock);
	io.AddKeyEvent(ImGuiKey_NumLock, m_Renderer.getKeyboard().m_NumLock);
	io.AddKeyEvent(ImGuiKey_PrintScreen, m_Renderer.getKeyboard().m_PrintScreen);
	io.AddKeyEvent(ImGuiKey_Pause, m_Renderer.getKeyboard().m_Pause);

	io.AddKeyEvent(ImGuiKey_F1, m_Renderer.getKeyboard().m_F1);
	io.AddKeyEvent(ImGuiKey_F2, m_Renderer.getKeyboard().m_F2);
	io.AddKeyEvent(ImGuiKey_F3, m_Renderer.getKeyboard().m_F3);
	io.AddKeyEvent(ImGuiKey_F4, m_Renderer.getKeyboard().m_F4);
	io.AddKeyEvent(ImGuiKey_F5, m_Renderer.getKeyboard().m_F5);
	io.AddKeyEvent(ImGuiKey_F6, m_Renderer.getKeyboard().m_F6);
	io.AddKeyEvent(ImGuiKey_F7, m_Renderer.getKeyboard().m_F7);
	io.AddKeyEvent(ImGuiKey_F8, m_Renderer.getKeyboard().m_F8);
	io.AddKeyEvent(ImGuiKey_F9, m_Renderer.getKeyboard().m_F9);
	io.AddKeyEvent(ImGuiKey_F10, m_Renderer.getKeyboard().m_F10);
	io.AddKeyEvent(ImGuiKey_F11, m_Renderer.getKeyboard().m_F11);
	io.AddKeyEvent(ImGuiKey_F12, m_Renderer.getKeyboard().m_F12);

	io.AddKeyEvent(ImGuiKey_KeyPadEnter, m_Renderer.getKeyboard().m_KeyPadEnter);

	io.AddKeyEvent(ImGuiKey_LeftShift, m_Renderer.getKeyboard().m_LeftShift);
	io.AddKeyEvent(ImGuiKey_LeftCtrl, m_Renderer.getKeyboard().m_LeftControl);
	io.AddKeyEvent(ImGuiKey_LeftAlt, m_Renderer.getKeyboard().m_LeftAlt);
	io.AddKeyEvent(ImGuiKey_LeftSuper, m_Renderer.getKeyboard().m_LeftSuper);

	io.AddKeyEvent(ImGuiKey_RightShift, m_Renderer.getKeyboard().m_RightShift);
	io.AddKeyEvent(ImGuiKey_RightCtrl, m_Renderer.getKeyboard().m_RightControl);
	io.AddKeyEvent(ImGuiKey_RightAlt, m_Renderer.getKeyboard().m_RightAlt);
	io.AddKeyEvent(ImGuiKey_RightSuper, m_Renderer.getKeyboard().m_RightSuper);

	io.AddKeyEvent(ImGuiKey_Menu, m_Renderer.getKeyboard().m_Menu);

	io.AddKeyEvent(ImGuiKey_A, m_Renderer.getKeyboard().m_KeyA);
	io.AddKeyEvent(ImGuiKey_B, m_Renderer.getKeyboard().m_KeyB);
	io.AddKeyEvent(ImGuiKey_C, m_Renderer.getKeyboard().m_KeyC);
	io.AddKeyEvent(ImGuiKey_D, m_Renderer.getKeyboard().m_KeyD);
	io.AddKeyEvent(ImGuiKey_E, m_Renderer.getKeyboard().m_KeyE);
	io.AddKeyEvent(ImGuiKey_F, m_Renderer.getKeyboard().m_KeyF);
	io.AddKeyEvent(ImGuiKey_G, m_Renderer.getKeyboard().m_KeyG);
	io.AddKeyEvent(ImGuiKey_H, m_Renderer.getKeyboard().m_KeyH);
	io.AddKeyEvent(ImGuiKey_I, m_Renderer.getKeyboard().m_KeyI);
	io.AddKeyEvent(ImGuiKey_J, m_Renderer.getKeyboard().m_KeyJ);
	io.AddKeyEvent(ImGuiKey_K, m_Renderer.getKeyboard().m_KeyK);
	io.AddKeyEvent(ImGuiKey_L, m_Renderer.getKeyboard().m_KeyL);
	io.AddKeyEvent(ImGuiKey_M, m_Renderer.getKeyboard().m_KeyM);
	io.AddKeyEvent(ImGuiKey_N, m_Renderer.getKeyboard().m_KeyN);
	io.AddKeyEvent(ImGuiKey_O, m_Renderer.getKeyboard().m_KeyO);
	io.AddKeyEvent(ImGuiKey_P, m_Renderer.getKeyboard().m_KeyP);
	io.AddKeyEvent(ImGuiKey_Q, m_Renderer.getKeyboard().m_KeyQ);
	io.AddKeyEvent(ImGuiKey_R, m_Renderer.getKeyboard().m_KeyR);
	io.AddKeyEvent(ImGuiKey_S, m_Renderer.getKeyboard().m_KeyS);
	io.AddKeyEvent(ImGuiKey_T, m_Renderer.getKeyboard().m_KeyT);
	io.AddKeyEvent(ImGuiKey_U, m_Renderer.getKeyboard().m_KeyU);
	io.AddKeyEvent(ImGuiKey_V, m_Renderer.getKeyboard().m_KeyV);
	io.AddKeyEvent(ImGuiKey_W, m_Renderer.getKeyboard().m_KeyW);
	io.AddKeyEvent(ImGuiKey_X, m_Renderer.getKeyboard().m_KeyX);
	io.AddKeyEvent(ImGuiKey_Y, m_Renderer.getKeyboard().m_KeyY);
	io.AddKeyEvent(ImGuiKey_Z, m_Renderer.getKeyboard().m_KeyZ);

	// Finally, setup the dockspace and everything else.
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground
	);

	ImGui::PopStyleVar(3);

	// Show the main menu.
	showMainMenu();

	ImGui::DockSpace(ImGui::GetID("EditorDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	// Show all the UI components.
	showUIs(delta);

	return !(io.WantCaptureMouse || io.WantCaptureKeyboard) || m_UIStorage.m_LayerViewUI.isInFocus();
}

void ImGuiLayer::endFrame() const
{
	ImGui::End();
	ImGui::Render();
}

void ImGuiLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
{
	m_pCommandRecorder->begin();

	// Copy the vertex and index data if necessary.
	prepareResources(m_pCommandRecorder.get(), frameIndex);

	// Draw.
	const auto* pDrawData = ImGui::GetDrawData();
	if (!pDrawData || pDrawData->CmdListsCount == 0)
	{
		m_pCommandRecorder->end();
		return;
	}

	m_pCommandRecorder->bind(m_pRasterizer.get(), m_ClearValues);
	m_pCommandRecorder->bind(m_pPipeline.get(), m_VertexSpecification);

	const auto& io = ImGui::GetIO();
	m_UserData.m_Scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	m_UserData.m_Translate = glm::vec2(-1.0f - pDrawData->DisplayPos.x * m_UserData.m_Scale.x, -1.0f - pDrawData->DisplayPos.y * m_UserData.m_Scale.y);
	m_pUniformBuffer->write(Xenon::ToBytes(&m_UserData), sizeof(UserData), 0, m_pCommandRecorder.get());

	m_pCommandRecorder->bind(m_pVertexBuffers[frameIndex].get(), sizeof(ImDrawVert));
	m_pCommandRecorder->bind(m_pIndexBuffers[frameIndex].get(), static_cast<Xenon::Backend::IndexBufferStride>(sizeof(ImDrawIdx)));
	m_pCommandRecorder->setViewportNatural(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, 1.0f);

	uint64_t indexOffset = 0;
	uint64_t vertexOffset = 0;
	for (uint32_t i = 0; i < pDrawData->CmdListsCount; i++)
	{
		const auto* pCommandList = pDrawData->CmdLists[i];
		for (uint32_t j = 0; j < pCommandList->CmdBuffer.Size; j++)
		{
			const auto* pCommandBuffer = &pCommandList->CmdBuffer[j];

			const auto minClip = glm::vec2(pCommandBuffer->ClipRect.x - pDrawData->DisplayPos.x, pCommandBuffer->ClipRect.y - pDrawData->DisplayPos.y);
			const auto maxClip = glm::vec2(pCommandBuffer->ClipRect.z - pDrawData->DisplayPos.x, pCommandBuffer->ClipRect.w - pDrawData->DisplayPos.y);
			if (maxClip.x <= minClip.x || maxClip.y <= minClip.y)
				continue;

			m_pCommandRecorder->setScissor(
				std::max(static_cast<int32_t>(minClip.x), 0),
				std::max(static_cast<int32_t>(minClip.y), 0),
				static_cast<uint32_t>(maxClip.x),
				static_cast<uint32_t>(maxClip.y)
			);

			m_pCommandRecorder->bind(m_pPipeline.get(), m_pUserDescriptor.get(), m_pMaterialDescriptors[XENON_BIT_CAST(uintptr_t, pCommandBuffer->TextureId)].get(), nullptr, nullptr);
			m_pCommandRecorder->drawIndexed(pCommandBuffer->VtxOffset + vertexOffset, pCommandBuffer->IdxOffset + indexOffset, pCommandBuffer->ElemCount);
		}

		indexOffset += pCommandList->IdxBuffer.Size;
		vertexOffset += pCommandList->VtxBuffer.Size;
	}

	m_pCommandRecorder->end();
}

void ImGuiLayer::showLayer(Xenon::Layer* pLayer)
{
	m_UIStorage.m_LayerViewUI.setLayer(pLayer);
}

void ImGuiLayer::setDrawCallCount(uint64_t totalCount, uint64_t actualCount)
{
	m_UIStorage.m_PerformanceMetricsUI.setDrawCallCount(totalCount, actualCount);
}

uintptr_t ImGuiLayer::getImageID(Xenon::Backend::Image* pImage, Xenon::Backend::ImageView* pImageView, Xenon::Backend::ImageSampler* pImageSampler)
{
	const std::array<uintptr_t, 3> pointers = { XENON_BIT_CAST(uintptr_t, pImage), XENON_BIT_CAST(uintptr_t, pImageView), XENON_BIT_CAST(uintptr_t, pImageSampler) };
	const auto ID = Xenon::GenerateHash(Xenon::ToBytes(pointers.data()), pointers.size() * sizeof(uintptr_t));
	if (!m_pMaterialDescriptors.contains(ID))
	{
		const auto& pDescriptor = m_pMaterialDescriptors[ID] = m_pPipeline->createDescriptor(Xenon::Backend::DescriptorType::Material);
		pDescriptor->attach(0, pImage, pImageView, pImageSampler, Xenon::Backend::ImageUsage::Graphics);
	}

	return ID;
}

void ImGuiLayer::disableClosing()
{
	m_bIsClosingDisabled = true;
}

void ImGuiLayer::enableClosing()
{
	m_bIsClosingDisabled = false;
}

void ImGuiLayer::configureImGui() const
{
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = static_cast<float>(m_Renderer.getWindow()->getWidth());
	io.DisplaySize.y = static_cast<float>(m_Renderer.getWindow()->getHeight());

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	io.MouseDrawCursor = true;

	// Setup the styles.
	auto& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_TitleBg] = ImVec4(CreateColor256(26), CreateColor256(30), CreateColor256(35), 0.5f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(CreateColor256(26), CreateColor256(30), CreateColor256(35), 0.75f);

	style.Colors[ImGuiCol_WindowBg] = ImVec4(CreateColor256(26), CreateColor256(30), CreateColor256(35), 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(CreateColor256(26), CreateColor256(30), CreateColor256(35), 1.0f);

	style.Colors[ImGuiCol_Header] = ImVec4(CreateColor256(25), CreateColor256(133), CreateColor256(161), 0.5f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(CreateColor256(25), CreateColor256(133), CreateColor256(161), 1.0f);

	style.Colors[ImGuiCol_Tab] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 0.25f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 0.75f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 0.5f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 0.25f);

	style.ChildRounding = 6.0f;
	style.FrameRounding = 1.0f;
	style.FramePadding.x = 5.0f;
	style.FramePadding.y = 2.0f;
	style.PopupRounding = 3.0f;
	style.TabRounding = 1.0f;
	style.WindowRounding = 3.0f;
	style.ItemInnerSpacing = ImVec2(2, 1);

	io.Fonts->AddFontFromFileTTF((std::filesystem::current_path() / "Fonts" / "Roboto" / "Roboto-Regular.ttf").string().c_str(), 14.0f);
}

void ImGuiLayer::setupDefaultMaterial()
{
	// Create the default material.
	auto& io = ImGui::GetIO();

	// Get the pixel data.
	unsigned char* pPixelData = nullptr;
	int32_t width = 0;
	int32_t height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pPixelData, &width, &height);

	// Create the staging buffer.
	const auto imageSize = static_cast<uint64_t>(width) * height * sizeof(unsigned char[4]);
	auto pBuffer = m_Renderer.getInstance().getFactory()->createBuffer(m_Renderer.getInstance().getBackendDevice(), imageSize, Xenon::Backend::BufferType::Staging);

	// Copy the data to it.
	pBuffer->write(Xenon::ToBytes(pPixelData), imageSize);

	// Create the image.
	Xenon::Backend::ImageSpecification imageSpecification = {};
	imageSpecification.m_Width = width;
	imageSpecification.m_Height = height;
	imageSpecification.m_Format = Xenon::Backend::DataFormat::R8G8B8A8_UNORMAL;
	m_pImage = m_Renderer.getInstance().getFactory()->createImage(m_Renderer.getInstance().getBackendDevice(), imageSpecification);

	// Copy the image data from the buffer to the image.
	m_pImage->copyFrom(pBuffer.get());

	// Create the image view.
	m_pImageView = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), m_pImage.get(), {});

	// Create the image sampler.
	Xenon::Backend::ImageSamplerSpecification imageSamplerSpecification = {};
	imageSamplerSpecification.m_AddressModeU = Xenon::Backend::AddressMode::ClampToEdge;
	imageSamplerSpecification.m_AddressModeV = Xenon::Backend::AddressMode::ClampToEdge;
	imageSamplerSpecification.m_AddressModeW = Xenon::Backend::AddressMode::ClampToEdge;
	imageSamplerSpecification.m_BorderColor = Xenon::Backend::BorderColor::OpaqueWhiteFLOAT;
	imageSamplerSpecification.m_MaxLevelOfDetail = 1.0f;
	m_pSampler = m_Renderer.getInstance().getFactory()->createImageSampler(m_Renderer.getInstance().getBackendDevice(), imageSamplerSpecification);

	// Create the descriptor.
	const auto& pDescriptor = m_pMaterialDescriptors[g_DefaultMaterialHash] = m_pPipeline->createDescriptor(Xenon::Backend::DescriptorType::Material);
	pDescriptor->attach(0, m_pImage.get(), m_pImageView.get(), m_pSampler.get(), Xenon::Backend::ImageUsage::Graphics);
}

void ImGuiLayer::setupPipeline()
{
	Xenon::Backend::ColorBlendAttachment attachment = {};
	attachment.m_EnableBlend = true;
	attachment.m_SrcBlendFactor = Xenon::Backend::ColorBlendFactor::SourceAlpha;
	attachment.m_DstBlendFactor = Xenon::Backend::ColorBlendFactor::OneMinusSourceAlpha;
	attachment.m_BlendOperator = Xenon::Backend::ColorBlendOperator::Add;
	attachment.m_SrcAlphaBlendFactor = Xenon::Backend::ColorBlendFactor::OneMinusSourceAlpha;
	attachment.m_DstAlphaBlendFactor = Xenon::Backend::ColorBlendFactor::Zero;
	attachment.m_AlphaBlendOperator = Xenon::Backend::ColorBlendOperator::Add;

	Xenon::Backend::RasterizingPipelineSpecification specification = {};
	specification.m_VertexShader = Xenon::Generated::CreateShaderImGuiLayer_vert();
	specification.m_FragmentShader = Xenon::Generated::CreateShaderImGuiLayer_frag();
	specification.m_CullMode = Xenon::Backend::CullMode::None;
	specification.m_ColorBlendAttachments = { attachment };
	specification.m_DepthCompareLogic = Xenon::Backend::DepthCompareLogic::Always;
	specification.m_EnableDepthTest = false;
	specification.m_EnableDepthWrite = false;

	// Create the pipeline.
	m_pPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(
		m_Renderer.getInstance().getBackendDevice(),
		std::make_unique<Xenon::DefaultCacheHandler>(),
		m_pRasterizer.get(),
		specification
	);

	// Create the user descriptor.
	m_pUserDescriptor = m_pPipeline->createDescriptor(Xenon::Backend::DescriptorType::UserDefined);

	// Create the uniform buffer and attach it to the descriptor.
	m_pUniformBuffer = m_Renderer.getInstance().getFactory()->createBuffer(m_Renderer.getInstance().getBackendDevice(), sizeof(UserData), Xenon::Backend::BufferType::Uniform);
	m_pUserDescriptor->attach(0, m_pUniformBuffer.get());

	// Setup the vertex specification.
	m_VertexSpecification.addElement(Xenon::Backend::InputElement::VertexPosition, Xenon::Backend::AttributeDataType::Vec2, Xenon::Backend::ComponentDataType::Float);
	m_VertexSpecification.addElement(Xenon::Backend::InputElement::VertexTextureCoordinate_0, Xenon::Backend::AttributeDataType::Vec2, Xenon::Backend::ComponentDataType::Float);
	m_VertexSpecification.addElement(Xenon::Backend::InputElement::VertexColor_0, Xenon::Backend::AttributeDataType::Vec4, Xenon::Backend::ComponentDataType::Uint8);
}

void ImGuiLayer::prepareResources(Xenon::Backend::CommandRecorder* pCommandRecorder, uint32_t frameIndex)
{
	// Copy the layer view.
	m_UIStorage.m_LayerViewUI.copyLayerImage(pCommandRecorder);

	const auto* pDrawData = ImGui::GetDrawData();

	// Return if we don't have any draw data.
	if (pDrawData == nullptr || !pDrawData->Valid)
		return;

	const auto vertexBufferSize = pDrawData->TotalVtxCount * sizeof(ImDrawVert);
	const auto indexBufferSize = pDrawData->TotalIdxCount * sizeof(ImDrawIdx);

	// Return if we don't have any vertex or index data.
	if (vertexBufferSize == 0 || indexBufferSize == 0)
		return;

	auto& pVertexBuffer = m_pVertexBuffers[frameIndex];
	auto& pIndexBuffer = m_pIndexBuffers[frameIndex];

	// Setup the vertex buffer if necessary.
	if (!pVertexBuffer || pVertexBuffer->getSize() != vertexBufferSize)
		pVertexBuffer = m_Renderer.getInstance().getFactory()->createBuffer(m_Renderer.getInstance().getBackendDevice(), vertexBufferSize, Xenon::Backend::BufferType::Vertex);

	// Setup the index buffer if necessary.
	if (!pIndexBuffer || pIndexBuffer->getSize() != indexBufferSize)
		pIndexBuffer = m_Renderer.getInstance().getFactory()->createBuffer(m_Renderer.getInstance().getBackendDevice(), indexBufferSize, Xenon::Backend::BufferType::Index);

	// Copy the data.
	uint64_t vertexOffset = 0;
	uint64_t indexOffset = 0;
	for (int32_t i = 0; i < pDrawData->CmdListsCount; i++)
	{
		const auto* pCommandList = pDrawData->CmdLists[i];

		const auto vertexCopySize = static_cast<uint64_t>(pCommandList->VtxBuffer.Size) * sizeof(ImDrawVert);
		const auto indexCopySize = static_cast<uint64_t>(pCommandList->IdxBuffer.Size) * sizeof(ImDrawIdx);

		pVertexBuffer->write(Xenon::ToBytes(pCommandList->VtxBuffer.Data), vertexCopySize, vertexOffset, pCommandRecorder);
		pIndexBuffer->write(Xenon::ToBytes(pCommandList->IdxBuffer.Data), indexCopySize, indexOffset, pCommandRecorder);

		vertexOffset += vertexCopySize;
		indexOffset += indexCopySize;
	}
}

uint64_t ImGuiLayer::getNextBufferSize(uint64_t requiredSize) const
{
	return requiredSize + 1024;
}

void ImGuiLayer::showMainMenu()
{
	ImGui::BeginMenuBar();

	showFileMenu();
	showEditMenu();
	showViewMenu();
	showHelpMenu();

	ImGui::EndMenuBar();
}

void ImGuiLayer::showFileMenu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open", "Ctrl+O")) Xenon::NoOp();
		if (ImGui::MenuItem("Save", "Ctrl+S")) Xenon::NoOp();
		if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) Xenon::NoOp();

		ImGui::Separator();
		if (ImGui::MenuItem("Close", nullptr, false, !m_bIsClosingDisabled))
		{
			m_Renderer.close();
			StudioConfiguration::GetInstance().toggleExitApplication();
		}

		const auto currentBackendType = StudioConfiguration::GetInstance().getCurrentBackendType();

		ImGui::Separator();
		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::Selectable("Vulkan Backend", currentBackendType == Xenon::BackendType::Vulkan, m_bIsClosingDisabled ? ImGuiSelectableFlags_Disabled : 0) && currentBackendType != Xenon::BackendType::Vulkan)
			{
				StudioConfiguration::GetInstance().setCurrentBackendType(Xenon::BackendType::Vulkan);
				m_Renderer.close();
			}

			if (ImGui::Selectable("DirectX 12 Backend", currentBackendType == Xenon::BackendType::DirectX_12, m_bIsClosingDisabled ? ImGuiSelectableFlags_Disabled : 0) && currentBackendType != Xenon::BackendType::DirectX_12)
			{
				StudioConfiguration::GetInstance().setCurrentBackendType(Xenon::BackendType::DirectX_12);
				m_Renderer.close();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void ImGuiLayer::showEditMenu()
{
	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Cut", "Ctrl+X")) Xenon::NoOp();
		if (ImGui::MenuItem("Copy", "Ctrl+C")) Xenon::NoOp();
		if (ImGui::MenuItem("Paste", "Ctrl+V")) Xenon::NoOp();

		ImGui::EndMenu();
	}
}

void ImGuiLayer::showViewMenu()
{
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("Layer View", "Ctrl+L", nullptr, !m_UIStorage.m_LayerViewUI.isVisible())) m_UIStorage.m_LayerViewUI.show();
		if (ImGui::MenuItem("Configuration", "Ctrl+R", nullptr, !m_UIStorage.m_ConfigurationUI.isVisible())) m_UIStorage.m_ConfigurationUI.show();
		if (ImGui::MenuItem("Performance Metrics", "Ctrl+P", nullptr, !m_UIStorage.m_PerformanceMetricsUI.isVisible())) m_UIStorage.m_PerformanceMetricsUI.show();
		if (ImGui::MenuItem("Pipeline Editor", "Ctrl+E", nullptr, !m_UIStorage.m_PipelineEditorUI.isVisible())) m_UIStorage.m_PipelineEditorUI.show();
		if (ImGui::MenuItem("Logs", "Ctrl+L", nullptr, !m_UIStorage.m_pLogs->isVisible())) m_UIStorage.m_pLogs->show();

		ImGui::EndMenu();
	}
}

void ImGuiLayer::showHelpMenu()
{
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("What's New?")) Xenon::NoOp();

		ImGui::Separator();
		if (ImGui::MenuItem("About")) Xenon::NoOp();
		if (ImGui::MenuItem("License")) Xenon::NoOp();

		ImGui::Separator();
		ImGui::MenuItem("Version: 1.0.0", "", false, false);

		ImGui::EndMenu();
	}
}

void ImGuiLayer::showUIs(std::chrono::nanoseconds delta)
{
	if (m_bShouldShowGreetingPopUp)
		ImGui::OpenPopup(g_WelcomePopUpID);

	m_UIStorage.m_LayerViewUI.begin(delta);
	m_UIStorage.m_LayerViewUI.end();

	m_UIStorage.m_ConfigurationUI.begin(delta);
	m_UIStorage.m_ConfigurationUI.end();

	m_UIStorage.m_PerformanceMetricsUI.begin(delta);
	m_UIStorage.m_PerformanceMetricsUI.end();

	m_UIStorage.m_PipelineEditorUI.begin(delta);
	m_UIStorage.m_PipelineEditorUI.end();

	m_UIStorage.m_pLogs->begin(delta);
	m_UIStorage.m_pLogs->end();

	for (auto& popUp : m_PopUpList)
	{
		popUp.begin(delta);
		popUp.end();
	}
}
