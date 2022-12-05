// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGuiLayer.hpp"
#include "../CacheHandler.hpp"
#include "../Materials/ImGuiMaterial.hpp"

#include "Xenon/Renderer.hpp"

#include <imgui.h>

constexpr auto g_DefaultMaterialHash = 0;

ImGuiLayer::ImGuiLayer(Xenon::Renderer& renderer, Xenon::Backend::Camera* pCamera)
	: RasterizingLayer(renderer, pCamera, Xenon::Backend::AttachmentType::Color)
	, m_pVertexBuffers(renderer.getCommandRecorder()->getBufferCount())
	, m_pIndexBuffers(renderer.getCommandRecorder()->getBufferCount())
{
	ImGui::CreateContext();

	configureImGui();
	setupDefaultMaterial();
}

ImGuiLayer::~ImGuiLayer()
{
	ImGui::DestroyContext();
}

void ImGuiLayer::beginFrame() const
{
	ImGui::NewFrame();

	// Process the ImGui events.
	auto& io = ImGui::GetIO();
	switch (m_Renderer.getMouse().m_ButtonLeft)
	{
	case Xenon::MouseButtonEvent::Release:
		io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
		break;

	case Xenon::MouseButtonEvent::Press:
		io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
		break;

	case Xenon::MouseButtonEvent::DoublePress:
		io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
		break;

	default:
		break;
	}

	switch (m_Renderer.getMouse().m_ButtonMiddle)
	{
	case Xenon::MouseButtonEvent::Release:
		io.AddMouseButtonEvent(ImGuiMouseButton_Middle, false);
		break;

	case Xenon::MouseButtonEvent::Press:
		io.AddMouseButtonEvent(ImGuiMouseButton_Middle, true);
		break;

	case Xenon::MouseButtonEvent::DoublePress:
		io.AddMouseButtonEvent(ImGuiMouseButton_Middle, true);
		break;

	default:
		break;
	}

	switch (m_Renderer.getMouse().m_ButtonRight)
	{
	case Xenon::MouseButtonEvent::Release:
		io.AddMouseButtonEvent(ImGuiMouseButton_Right, false);
		break;

	case Xenon::MouseButtonEvent::Press:
		io.AddMouseButtonEvent(ImGuiMouseButton_Right, true);
		break;

	case Xenon::MouseButtonEvent::DoublePress:
		io.AddMouseButtonEvent(ImGuiMouseButton_Right, true);
		break;

	default:
		break;
	}

	io.AddMouseWheelEvent(m_Renderer.getMouse().m_HScroll, m_Renderer.getMouse().m_VScroll);
	io.AddMousePosEvent(m_Renderer.getMouse().m_MousePosition.m_XAxis, m_Renderer.getMouse().m_MousePosition.m_YAxis);
	io.AddInputCharacter(m_Renderer.getKeyboard().m_Character);
}

void ImGuiLayer::endFrame() const
{
	ImGui::Render();
}

void ImGuiLayer::bind(Xenon::Layer* pPreviousLayer, Xenon::Backend::CommandRecorder* pCommandRecorder)
{
	// Copy the vertex and index data if necessary.
	prepareResources(pCommandRecorder);

	// Draw.
	const auto* pDrawData = ImGui::GetDrawData();
	if (!pDrawData || pDrawData->CmdListsCount == 0)
		return;

	pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) });
	pCommandRecorder->bind(m_pPipeline.get(), m_VertexSpecification);

	const auto& io = ImGui::GetIO();
	m_UserData.m_Scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	m_UserData.m_Translate = glm::vec2(-1.0f);
	m_pUniformBuffer->write(Xenon::ToBytes(&m_UserData), sizeof(UserData), 0, pCommandRecorder);

	const auto frameIndex = pCommandRecorder->getCurrentIndex();
	pCommandRecorder->bind(m_pVertexBuffers[frameIndex].get(), sizeof(ImDrawVert));
	pCommandRecorder->bind(m_pIndexBuffers[frameIndex].get(), static_cast<Xenon::Backend::IndexBufferStride>(sizeof(ImDrawIdx)));
	pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);

	uint32_t indexOffset = 0;
	uint32_t vertexOffset = 0;
	for (uint32_t i = 0; i < pDrawData->CmdListsCount; i++)
	{
		const auto* pCommandList = pDrawData->CmdLists[i];
		for (uint32_t j = 0; j < pCommandList->CmdBuffer.Size; j++)
		{
			const auto* pCommandBuffer = &pCommandList->CmdBuffer[j];

			pCommandRecorder->setScissor(
				std::max(static_cast<int32_t>(pCommandBuffer->ClipRect.x), 0),
				std::max(static_cast<int32_t>(pCommandBuffer->ClipRect.y), 0),
				static_cast<uint32_t>(pCommandBuffer->ClipRect.z - pCommandBuffer->ClipRect.x),
				static_cast<uint32_t>(pCommandBuffer->ClipRect.w - pCommandBuffer->ClipRect.y)
			);

			pCommandRecorder->bind(m_pPipeline.get(), m_pUserDescriptor.get(), m_pDescriptorSetMap[std::bit_cast<uint64_t>(pCommandBuffer->TextureId)].get(), nullptr);
			pCommandRecorder->drawIndexed(pCommandBuffer->VtxOffset + vertexOffset, pCommandBuffer->IdxOffset + indexOffset, pCommandBuffer->ElemCount);
		}

		indexOffset += pCommandList->IdxBuffer.Size;
		vertexOffset += pCommandList->VtxBuffer.Size;
	}
}

void ImGuiLayer::configureImGui()
{
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = static_cast<float>(m_Renderer.getCamera()->getWidth());
	io.DisplaySize.y = static_cast<float>(m_Renderer.getCamera()->getHeight());
}

void ImGuiLayer::setupDefaultMaterial()
{
	// Create the default material.
	m_DefaultMaterialIdentifier = m_Renderer.getInstance().getMaterialDatabase().create<ImGuiMaterial>(g_DefaultMaterialHash, m_Renderer.getInstance());

	// Create the pipeline.
	m_pPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(
		m_Renderer.getInstance().getBackendDevice(),
		std::make_unique<CacheHandler>(),
		m_pRasterizer.get(),
		m_DefaultMaterialIdentifier.m_pMaterial->getRasterizingSpecification()
	);

	// Create the default descriptor set.
	m_pDescriptorSetMap[0] = m_DefaultMaterialIdentifier.m_pMaterial->createDescriptor(m_pPipeline.get());

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

void ImGuiLayer::prepareResources(Xenon::Backend::CommandRecorder* pCommandRecorder)
{
	const auto* pDrawData = ImGui::GetDrawData();

	// Return if we don't have any draw data.
	if (pDrawData == nullptr || !pDrawData->Valid)
		return;

	const auto vertexBufferSize = pDrawData->TotalVtxCount * sizeof(ImDrawVert);
	const auto indexBufferSize = pDrawData->TotalIdxCount * sizeof(ImDrawIdx);

	// Return if we don't have any vertex or index data.
	if (vertexBufferSize == 0 || indexBufferSize == 0)
		return;

	// Setup the vertex buffer if necessary.
	const auto frameIndex = pCommandRecorder->getCurrentIndex();
	if (!m_pVertexBuffers[frameIndex] || m_pVertexBuffers[frameIndex]->getSize() < vertexBufferSize)
		m_pVertexBuffers[frameIndex] = m_Renderer.getInstance().getFactory()->createBuffer(m_Renderer.getInstance().getBackendDevice(), getNextBufferSize(vertexBufferSize), Xenon::Backend::BufferType::Vertex);

	// Setup the index buffer if necessary.
	if (!m_pIndexBuffers[frameIndex] || m_pIndexBuffers[frameIndex]->getSize() < indexBufferSize)
		m_pIndexBuffers[frameIndex] = m_Renderer.getInstance().getFactory()->createBuffer(m_Renderer.getInstance().getBackendDevice(), getNextBufferSize(indexBufferSize), Xenon::Backend::BufferType::Index);

	// Copy the data.
	uint64_t vertexOffset = 0;
	uint64_t indexOffset = 0;
	for (int32_t i = 0; i < pDrawData->CmdListsCount; i++)
	{
		const auto* pCommandList = pDrawData->CmdLists[i];

		m_pVertexBuffers[frameIndex]->write(Xenon::ToBytes(pCommandList->VtxBuffer.Data), static_cast<uint64_t>(pCommandList->VtxBuffer.Size) * sizeof(ImDrawVert), vertexOffset, pCommandRecorder);
		m_pIndexBuffers[frameIndex]->write(Xenon::ToBytes(pCommandList->IdxBuffer.Data), static_cast<uint64_t>(pCommandList->IdxBuffer.Size) * sizeof(ImDrawIdx), indexOffset, pCommandRecorder);

		vertexOffset += pCommandList->VtxBuffer.Size * sizeof(ImDrawVert);
		indexOffset += pCommandList->IdxBuffer.Size * sizeof(ImDrawIdx);
	}
}

uint64_t ImGuiLayer::getNextBufferSize(uint64_t requiredSize) const
{
	return requiredSize + 1024;
}
