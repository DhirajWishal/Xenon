// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../MaterialBlob.hpp"

namespace Xenon
{
	/**
	 * PBR metallic roughness material.
	 */
	class PBRMetallicRoughnessMaterial final : public MaterialBlob
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param pImage The image pointer.
		 * @param pImagView The image view pointer.
		 * @param pImageSampler The image sampler pointer.
		 */
		explicit PBRMetallicRoughnessMaterial(Instance& instance, std::unique_ptr<Backend::Image>&& pImage, std::unique_ptr<Backend::ImageView>&& pImageView, std::unique_ptr<Backend::ImageSampler>&& pImageSampler);

		/**
		 * Get the rasterizing pipeline specification.
		 *
		 * @return The pipeline specification.
		 */
		[[nodiscard]] Backend::RasterizingPipelineSpecification getRasterizingSpecification() override;

		/**
		 * Create the material descriptor for the current material.
		 *
		 * @param pPipeline The pipeline pointer to create the material descriptor from.
		 * @return The material pointer.
		 */
		[[nodiscard]] std::unique_ptr<Backend::Descriptor> createDescriptor(Backend::Pipeline* pPipeline) override;

	private:
		Backend::RasterizingPipelineSpecification m_RasterizingPipelineSpecification = {};

		std::unique_ptr<Backend::Image> m_pImage = nullptr;
		std::unique_ptr<Backend::ImageView> m_pImageView = nullptr;
		std::unique_ptr<Backend::ImageSampler> m_pSampler = nullptr;
	};
}