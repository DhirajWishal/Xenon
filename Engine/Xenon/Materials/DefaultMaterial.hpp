// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../MaterialBlob.hpp"

namespace Xenon
{
	/**
	 * Default material.
	 */
	class DefaultMaterial final : public MaterialBlob
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 */
		explicit DefaultMaterial(Instance& instance);

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