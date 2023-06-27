// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Material.hpp"

namespace Xenon
{
	Xenon::MaterialBuilder& MaterialBuilder::setRasterizingPipelineSpecification(const Backend::RasterizingPipelineSpecification& specification)
	{
		m_MaterialSpecification.m_RasterizingPipelineSpecification = specification;
		return *this;
	}

	Xenon::MaterialBuilder& MaterialBuilder::setRayTracingPipelineSpecification(const Backend::RayTracingPipelineSpecification& specification)
	{
		m_MaterialSpecification.m_RayTracingPipelineSpecification = specification;
		return *this;
	}

	Xenon::MaterialBuilder& MaterialBuilder::addBaseColorTexture(Texture payload /*= {}*/)
	{
		m_MaterialSpecification.m_Properties.emplace_back(payload, MaterialPropertyType::BaseColorTexture);
		return *this;
	}

	Xenon::MaterialBuilder& MaterialBuilder::addRoughnessTexture(Texture payload /*= {}*/)
	{
		m_MaterialSpecification.m_Properties.emplace_back(payload, MaterialPropertyType::RoughnessTexture);
		return *this;
	}

	Xenon::MaterialBuilder& MaterialBuilder::addNormalTexture(Texture payload /*= {}*/)
	{
		m_MaterialSpecification.m_Properties.emplace_back(payload, MaterialPropertyType::NormalTexture);
		return *this;
	}

	Xenon::MaterialBuilder& MaterialBuilder::addOcclusionTexture(Texture payload /*= {}*/)
	{
		m_MaterialSpecification.m_Properties.emplace_back(payload, MaterialPropertyType::OcclusionTexture);
		return *this;
	}

	Xenon::MaterialBuilder& MaterialBuilder::addEmissiveTexture(Texture payload /*= {}*/)
	{
		m_MaterialSpecification.m_Properties.emplace_back(payload, MaterialPropertyType::EmissiveTexture);
		return *this;
	}

	const Xenon::Backend::RasterizingPipelineSpecification& MaterialBuilder::getRasterizingPipelineSpecification() const noexcept
	{
		return m_MaterialSpecification.m_RasterizingPipelineSpecification;
	}

	const Xenon::Backend::RayTracingPipelineSpecification& MaterialBuilder::getRayTracingPipelineSpecification() const noexcept
	{
		return m_MaterialSpecification.m_RayTracingPipelineSpecification;
	}

	MaterialBuilder::operator MaterialSpecification& () noexcept
	{
		return m_MaterialSpecification;
	}

	MaterialBuilder::operator const MaterialSpecification& () const noexcept
	{
		return m_MaterialSpecification;
	}
}