// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGuiLayer.hpp"
#include "../CacheHandler.hpp"

#include "Xenon/Renderer.hpp"

#include <imgui.h>

constexpr auto g_DefaultMaterialHash = 0;

ImGuiMaterial::ImGuiMaterial(Xenon::Instance& instance)
	: Xenon::MaterialBlob(instance)
	, m_pSampler(instance.getFactory()->createImageSampler(instance.getBackendDevice(), {}))
{
	ImGuiIO& io = ImGui::GetIO();

	// Get the pixel data.
	unsigned char* pPixelData = nullptr;
	int32_t width = 0;
	int32_t height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pPixelData, &width, &height);

	// Create the staging buffer.
	const auto imageSize = static_cast<uint64_t>(width) * height * sizeof(unsigned char[4]);
	auto pBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), imageSize, Xenon::Backend::BufferType::Staging);

	// Copy the data to it.
	pBuffer->write(Xenon::ToBytes(pPixelData), imageSize);

	// Create the image.
	Xenon::Backend::ImageSpecification imageSpecification = {};
	imageSpecification.m_Width = width;
	imageSpecification.m_Height = height;
	imageSpecification.m_Format = Xenon::Backend::DataFormat::R8G8B8A8_SRGB;

	m_pImage = instance.getFactory()->createImage(instance.getBackendDevice(), imageSpecification);

	// Copy the image data from the buffer to the image.
	m_pImage->copyFrom(pBuffer.get());

	// Create the image view.
	m_pImageView = instance.getFactory()->createImageView(instance.getBackendDevice(), m_pImage.get(), {});
}

ImGuiMaterial::ImGuiMaterial(Xenon::Instance& instance, Xenon::Backend::Image* pImage)
	: Xenon::MaterialBlob(instance)
	, m_pSampler(instance.getFactory()->createImageSampler(instance.getBackendDevice(), {}))
{

}

Xenon::Backend::RasterizingPipelineSpecification ImGuiMaterial::getRasterizingSpecification()
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
	specification.m_VertexShader = Xenon::Backend::ShaderSource::FromFile("Shaders/ImGuiLayer/Shader.vert.spv");
	specification.m_FragmentShader = Xenon::Backend::ShaderSource::FromFile("Shaders/ImGuiLayer/Shader.frag.spv");
	specification.m_ColorBlendAttachments = { attachment };
	specification.m_DepthCompareLogic = Xenon::Backend::DepthCompareLogic::Always;

	return specification;
}

std::unique_ptr<Xenon::Backend::Descriptor> ImGuiMaterial::createDescriptor(Xenon::Backend::Pipeline* pPipeline)
{
	auto pDescriptor = pPipeline->createDescriptor(Xenon::Backend::DescriptorType::Material);
	pDescriptor->attach(0, m_pImage.get(), m_pImageView.get(), m_pSampler.get(), Xenon::Backend::ImageUsage::Graphics);

	return pDescriptor;
}

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
}

void ImGuiLayer::endFrame()
{
	ImGui::Render();

	// // Create the task to prepare the resources if we haven't already.
	// if (m_pTaskNode == nullptr)
	// 	m_pTaskNode = m_Renderer.getTaskGraph().create([this] { prepareResources(); });
	// 
	// // If we do, just reset it.
	// else
	// 	m_pTaskNode->reset();
}

void ImGuiLayer::bind(Xenon::Layer* pPreviousLayer, Xenon::Backend::CommandRecorder* pCommandRecorder)
{
	// Copy the vertex and index data if necessary.
	prepareResources(pCommandRecorder);

	// Draw.
	const auto* pDrawData = ImGui::GetDrawData();
	if (!pDrawData || pDrawData->CmdListsCount == 0)
		return;

	const auto& io = ImGui::GetIO();

	pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) });
	pCommandRecorder->bind(m_pPipeline.get(), m_VertexSpecification);

	m_UserData.m_Scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	m_UserData.m_Translate = glm::vec2(-1.0f);
	m_pUniformBuffer->write(Xenon::ToBytes(&m_UserData), sizeof(UserData), 0, pCommandRecorder);

	const auto frameIndex = pCommandRecorder->getCurrentIndex();
	pCommandRecorder->bind(m_pVertexBuffers[frameIndex].get(), sizeof(ImDrawVert));
	pCommandRecorder->bind(m_pIndexBuffers[frameIndex].get(), static_cast<Xenon::Backend::IndexBufferStride>(sizeof(ImDrawIdx)));

	uint32_t vertexOffset = 0;
	uint32_t indexOffset = 0;
	for (uint32_t i = 0; i < pDrawData->CmdListsCount; i++)
	{
		const auto* cmd_list = pDrawData->CmdLists[i];
		for (uint32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
		{
			const auto* pcmd = &cmd_list->CmdBuffer[j];

			pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
			pCommandRecorder->setScissor(
				std::max(static_cast<int32_t>(pcmd->ClipRect.x), 0),
				std::max(static_cast<int32_t>(pcmd->ClipRect.y), 0),
				static_cast<uint32_t>(pcmd->ClipRect.z - pcmd->ClipRect.x),
				static_cast<uint32_t>(pcmd->ClipRect.w - pcmd->ClipRect.y)
			);

			pCommandRecorder->bind(m_pPipeline.get(), m_pUserDescriptor.get(), m_pDescriptorSetMap[std::bit_cast<uint64_t>(pcmd->TextureId)].get(), nullptr);
			pCommandRecorder->drawIndexed(vertexOffset, indexOffset, pcmd->ElemCount);
			indexOffset += pcmd->ElemCount;
		}
		vertexOffset += cmd_list->VtxBuffer.Size;
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
	const auto frameIndex = pCommandRecorder->getCurrentIndex();
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
