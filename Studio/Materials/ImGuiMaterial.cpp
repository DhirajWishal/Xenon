// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGuiMaterial.hpp"
#include "Xenon/Renderer.hpp"

#include "XenonBackend/CommandRecorder.hpp"

#include <imgui.h>

ImGuiMaterial::ImGuiMaterial(Xenon::Instance& instance)
	: Xenon::MaterialBlob(instance)
{
	auto& io = ImGui::GetIO();

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
	imageSpecification.m_Format = Xenon::Backend::DataFormat::R8G8B8A8_UNORMAL;
	m_pImage = instance.getFactory()->createImage(instance.getBackendDevice(), imageSpecification);

	// Copy the image data from the buffer to the image.
	m_pImage->copyFrom(pBuffer.get());

	// Create the image view.
	m_pImageView = instance.getFactory()->createImageView(instance.getBackendDevice(), m_pImage.get(), {});

	// Create the image sampler.
	Xenon::Backend::ImageSamplerSpecification imageSamplerSpecification = {};
	imageSamplerSpecification.m_AddressModeU = Xenon::Backend::AddressMode::Repeat;
	imageSamplerSpecification.m_AddressModeV = Xenon::Backend::AddressMode::Repeat;
	imageSamplerSpecification.m_AddressModeW = Xenon::Backend::AddressMode::Repeat;
	imageSamplerSpecification.m_BorderColor = Xenon::Backend::BorderColor::OpaqueWhiteFLOAT;
	imageSamplerSpecification.m_MaxAnisotrophy = 1.0f;
	m_pSampler = instance.getFactory()->createImageSampler(instance.getBackendDevice(), imageSamplerSpecification);
}

ImGuiMaterial::ImGuiMaterial(Xenon::Instance& instance, const Xenon::Backend::Image* pImage)
	: Xenon::MaterialBlob(instance)
{
	// Create the image.
	Xenon::Backend::ImageSpecification imageSpecification = pImage->getSpecification();
	imageSpecification.m_Usage = Xenon::Backend::ImageUsage::Graphics;
	m_pImage = instance.getFactory()->createImage(instance.getBackendDevice(), imageSpecification);

	// Create the image view.
	m_pImageView = instance.getFactory()->createImageView(instance.getBackendDevice(), m_pImage.get(), {});

	// Create the image sampler.
	Xenon::Backend::ImageSamplerSpecification imageSamplerSpecification = {};
	imageSamplerSpecification.m_AddressModeU = Xenon::Backend::AddressMode::Repeat;
	imageSamplerSpecification.m_AddressModeV = Xenon::Backend::AddressMode::Repeat;
	imageSamplerSpecification.m_AddressModeW = Xenon::Backend::AddressMode::Repeat;
	imageSamplerSpecification.m_BorderColor = Xenon::Backend::BorderColor::OpaqueWhiteFLOAT;
	imageSamplerSpecification.m_MaxAnisotrophy = 1.0f;
	m_pSampler = instance.getFactory()->createImageSampler(instance.getBackendDevice(), imageSamplerSpecification);
}

Xenon::Backend::RasterizingPipelineSpecification ImGuiMaterial::getRasterizingSpecification()
{
	Xenon::Backend::ColorBlendAttachment attachment = {};
	attachment.m_EnableBlend = true;
	attachment.m_SrcBlendFactor = Xenon::Backend::ColorBlendFactor::SourceAlpha;
	attachment.m_DstBlendFactor = Xenon::Backend::ColorBlendFactor::OneMinusSourceAlpha;
	attachment.m_BlendOperator = Xenon::Backend::ColorBlendOperator::Add;
	attachment.m_SrcAlphaBlendFactor = Xenon::Backend::ColorBlendFactor::One;
	attachment.m_DstAlphaBlendFactor = Xenon::Backend::ColorBlendFactor::OneMinusSourceAlpha;
	attachment.m_AlphaBlendOperator = Xenon::Backend::ColorBlendOperator::Add;

	Xenon::Backend::RasterizingPipelineSpecification specification = {};
	specification.m_VertexShader = Xenon::Backend::ShaderSource::FromFile("Shaders/ImGuiLayer/Shader.vert.spv");
	specification.m_FragmentShader = Xenon::Backend::ShaderSource::FromFile("Shaders/ImGuiLayer/Shader.frag.spv");
	specification.m_ColorBlendAttachments = { attachment };
	specification.m_DepthCompareLogic = Xenon::Backend::DepthCompareLogic::Always;
	specification.m_CullMode = Xenon::Backend::CullMode::None;

	return specification;
}

std::unique_ptr<Xenon::Backend::Descriptor> ImGuiMaterial::createDescriptor(Xenon::Backend::Pipeline* pPipeline)
{
	auto pDescriptor = pPipeline->createDescriptor(Xenon::Backend::DescriptorType::Material);
	pDescriptor->attach(0, m_pImage.get(), m_pImageView.get(), m_pSampler.get(), Xenon::Backend::ImageUsage::Graphics);

	return pDescriptor;
}

void ImGuiMaterial::performCopy(Xenon::Backend::CommandRecorder* pCommandRecorder, Xenon::Backend::Image* pImage) const
{
	pCommandRecorder->copy(pImage, glm::vec3(0), m_pImage.get(), glm::vec3(0));
}
