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
	 * Material specification structure.
	 * This contains all the necessary information about a material.
	 */
	struct MaterialSpecification final
	{
		Backend::RasterizingPipelineSpecification m_RasterizingPipelineSpecification;
		Backend::RayTracingPipelineSpecification m_RayTracingPipelineSpecification;

		std::vector<MaterialProperty> m_Properties;
	};

	/**
	 * Material enum.
	 * This is what the user gets to play around with.
	 */
	enum class Material : uint64_t {};

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
		MaterialBuilder& setRasterizingPipelineSpecification(const Backend::RasterizingPipelineSpecification& specification);

		/**
		 * Set the ray tracing pipeline specification for the material.
		 *
		 * @param specification The specification.
		 * @return The material builder used to chain.
		 */
		MaterialBuilder& setRayTracingPipelineSpecification(const Backend::RayTracingPipelineSpecification& specification);

		/**
		 * Add a base color texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addBaseColorTexture(Texture payload = {});

		/**
		 * Add a roughness texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addRoughnessTexture(Texture payload = {});

		/**
		 * Add a normal texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addNormalTexture(Texture payload = {});

		/**
		 * Add a occlusion texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addOcclusionTexture(Texture payload = {});

		/**
		 * Add a emissive texture property to the builder.
		 *
		 * @param payload The texture payload. Default is null. Leave this empty to use the sub-mesh's texture.
		 * @return The builder reference used to chain.
		 */
		MaterialBuilder& addEmissiveTexture(Texture payload = {});

		/**
		 * Get the rasterizing pipeline specification.
		 *
		 * @return The specification reference.
		 */
		[[nodiscard]] const Backend::RasterizingPipelineSpecification& getRasterizingPipelineSpecification() const noexcept;

		/**
		 * Get the ray tracing pipeline specification.
		 *
		 * @return The specification reference.
		 */
		[[nodiscard]] const Backend::RayTracingPipelineSpecification& getRayTracingPipelineSpecification() const noexcept;

		/**
		 * Get the material specification.
		 *
		 * @return The material specification.
		 */
		[[nodiscard]] explicit operator MaterialSpecification& () noexcept;

		/**
		 * Get the material specification.
		 *
		 * @return The material specification.
		 */
		[[nodiscard]] explicit operator const MaterialSpecification& () const noexcept;

	private:
		MaterialSpecification m_MaterialSpecification;
	};

	/**
	 * Utility function to easily generate the hash for the material specification object.
	 *
	 * @param specification The material specification to generate the hash for.
	 * @param seed The hash seed. Default is 0.
	 * @return The 64-bit hash value.
	 */
	template<>
	[[nodiscard]] inline uint64_t GenerateHashFor<MaterialSpecification>(const MaterialSpecification& specification, uint64_t seed) noexcept
	{
		const auto rpsHash = GenerateHashFor(specification.m_RasterizingPipelineSpecification, seed);
		const auto rtpsHash = GenerateHashFor(specification.m_RayTracingPipelineSpecification, rpsHash);
		return GenerateHash(ToBytes(specification.m_Properties.data()), specification.m_Properties.size() * sizeof(MaterialProperty), rtpsHash);
	}
}