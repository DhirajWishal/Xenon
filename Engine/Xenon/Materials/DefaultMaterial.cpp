// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultMaterial.hpp"
#include "../Instance.hpp"

#include "../../XenonShaderBank/Default/Default.vert.hpp"
#include "../../XenonShaderBank/Default/Default.frag.hpp"

#include <optick.h>

namespace Xenon
{
	DefaultMaterial::DefaultMaterial(Instance& instance)
		: MaterialBlob(instance)
	{
		// Setup the default image, image view and sampler.
		Backend::ImageSpecification imageSpecification = {};
		imageSpecification.m_Width = 1;
		imageSpecification.m_Height = 1;
		imageSpecification.m_Format = Xenon::Backend::DataFormat::R8G8B8A8_SRGB;
		m_pImage = instance.getFactory()->createImage(instance.getBackendDevice(), imageSpecification);

		// Setup image view.
		Backend::ImageViewSpecification imageViewSpecification = {};
		m_pImageView = instance.getFactory()->createImageView(instance.getBackendDevice(), m_pImage.get(), imageViewSpecification);

		// Setup image sampler.
		Backend::ImageSamplerSpecification imageSamplerSpecification = {};
		m_pSampler = instance.getFactory()->createImageSampler(instance.getBackendDevice(), imageSamplerSpecification);

		// Setup the rasterizing pipeline specification.
		m_RasterizingPipelineSpecification.m_VertexShader = Generated::CreateShaderDefault_vert();
		m_RasterizingPipelineSpecification.m_FragmentShader = Generated::CreateShaderDefault_frag();
	}

	Xenon::Backend::RasterizingPipelineSpecification DefaultMaterial::getRasterizingSpecification()
	{
		return m_RasterizingPipelineSpecification;
	}

	std::unique_ptr<Xenon::Backend::Descriptor> DefaultMaterial::createDescriptor(Backend::Pipeline* pPipeline)
	{
		OPTICK_EVENT();

		auto pDescriptor = pPipeline->createDescriptor(Backend::DescriptorType::Material);
		pDescriptor->attach(0, m_pImage.get(), m_pImageView.get(), m_pSampler.get(), Xenon::Backend::ImageUsage::Graphics);

		return pDescriptor;
	}
}