// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "PBRMetallicRoughnessMaterial.hpp"
#include "../Instance.hpp"

#include <optick.h>

namespace Xenon
{
	PBRMetallicRoughnessMaterial::PBRMetallicRoughnessMaterial(Instance& instance, std::unique_ptr<Backend::Image>&& pImage, std::unique_ptr<Backend::ImageView>&& pImageView, std::unique_ptr<Backend::ImageSampler>&& pImageSampler)
		: MaterialBlob(instance)
		, m_pImage(std::move(pImage))
		, m_pImageView(std::move(pImageView))
		, m_pSampler(std::move(pImageSampler))
	{
		m_RasterizingPipelineSpecification.m_VertexShader = Xenon::Backend::Shader(Xenon::Backend::ShaderSource::FromFile(XENON_SHADER_DIR "PBRMetallicRoughness/Shader.vert.spv"));
		m_RasterizingPipelineSpecification.m_FragmentShader = Xenon::Backend::Shader(Xenon::Backend::ShaderSource::FromFile(XENON_SHADER_DIR "PBRMetallicRoughness/Shader.frag.spv"));
	}

	Xenon::Backend::RasterizingPipelineSpecification PBRMetallicRoughnessMaterial::getRasterizingSpecification()
	{
		return m_RasterizingPipelineSpecification;
	}

	std::unique_ptr<Xenon::Backend::Descriptor> PBRMetallicRoughnessMaterial::createDescriptor(Backend::Pipeline* pPipeline)
	{
		OPTICK_EVENT();

		auto pDescriptor = pPipeline->createDescriptor(Backend::DescriptorType::Material);
		pDescriptor->attach(0, m_pImage.get(), m_pImageView.get(), m_pSampler.get(), Xenon::Backend::ImageUsage::Graphics);

		return pDescriptor;
	}
}