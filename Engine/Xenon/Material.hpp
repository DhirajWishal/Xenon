// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RasterizingPipeline.hpp"
#include "../XenonBackend/RayTracingPipeline.hpp"

#include <variant>

namespace Xenon
{
	/**
	 * Texture structure.
	 * This structure contains information about a single texture and how to sample it.
	 */
	struct Texture final
	{
		Backend::Image* m_pImage = nullptr;
		Backend::ImageView* m_pImageView = nullptr;
		Backend::ImageSampler* m_pImageSampler = nullptr;
	};

	/**
	 * Material property type enum.
	 * This describes what the type of a property is.
	 */
	enum class MaterialPropertyType : uint8_t
	{
		BaseColorTexture,
		RoughnessTexture,
		NormalTexture,
		OcclusionTexture,
		EmissiveTexture,
	};

	/**
	 * Material payload type.
	 * This is a variant with all the possible types a single material property could have.
	 */
	using MaterialPayload = std::variant<
		Texture
	>;

	/**
	 * Property structure.
	 */
	struct MaterialProperty final
	{
		MaterialPayload m_Payload;
		MaterialPropertyType m_Type;
	};

	/**
	 * Material structure.
	 */
	struct Material final
	{
		Backend::RasterizingPipelineSpecification m_RasterizingPipelineSpecification;
		Backend::RayTracingPipelineSpecification m_RayTracingPipelineSpecification;

		std::vector<MaterialProperty> m_Properties;
	};

	/**
	 * Material builder class.
	 *
	 * Make sure to add properties in the same order they appear in the material descriptor.
	 */
	class MaterialBuilder final
	{
	public:
		/**
		 * Default constructor.
		 */
		MaterialBuilder() = default;

		/**
		 * Set the rasterizing pipeline specification for the material.
		 *
		 * @param specification The specification.
		 * @return The material builder used to chain.
		 */
		MaterialBuilder& setRasterizingPipelineSpecification(const Backend::RasterizingPipelineSpecification& specification) { m_Material.m_RasterizingPipelineSpecification = specification; return *this; }

		/**
		 * Set the ray tracing pipeline specification for the material.
		 *
		 * @param specification The specification.
		 * @return The material builder used to chain.
		 */
		MaterialBuilder& setRayTracingPipelineSpecification(const Backend::RayTracingPipelineSpecification& specification) { m_Material.m_RayTracingPipelineSpecification = specification; return *this; }

		/**
		 * Add a base color texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addBaseColorTexture(Texture payload = {}) { m_Material.m_Properties.emplace_back(payload, MaterialPropertyType::BaseColorTexture); return *this; }

		/**
		 * Add a roughness texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addRoughnessTexture(Texture payload = {}) { m_Material.m_Properties.emplace_back(payload, MaterialPropertyType::RoughnessTexture); return *this; }

		/**
		 * Add a normal texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addNormalTexture(Texture payload = {}) { m_Material.m_Properties.emplace_back(payload, MaterialPropertyType::NormalTexture); return *this; }

		/**
		 * Add a occlusion texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addOcclusionTexture(Texture payload = {}) { m_Material.m_Properties.emplace_back(payload, MaterialPropertyType::OcclusionTexture); return *this; }

		/**
		 * Add a emissive texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addEmissiveTexture(Texture payload = {}) { m_Material.m_Properties.emplace_back(payload, MaterialPropertyType::EmissiveTexture); return *this; }

		/**
		 * Get the rasterizing pipeline specification.
		 *
		 * @return The specification reference.
		 */
		[[nodiscard]] const Backend::RasterizingPipelineSpecification& getRasterizingPipelineSpecification() const noexcept { return m_Material.m_RasterizingPipelineSpecification; }

		/**
		 * Get the ray tracing pipeline specification.
		 *
		 * @return The specification reference.
		 */
		[[nodiscard]] const Backend::RayTracingPipelineSpecification& getRayTracingPipelineSpecification() const noexcept { return m_Material.m_RayTracingPipelineSpecification; }

		/**
		 * Get the created material.
		 *
		 * @return The material.
		 */
		[[nodiscard]] const Material& getMaterial() const noexcept { return m_Material; }

	private:
		Material m_Material;
	};
}