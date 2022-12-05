// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Xenon/MaterialBlob.hpp"

/**
 * ImGui material class.
 */
class ImGuiMaterial final : public Xenon::MaterialBlob
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param instance The instance reference.
	 */
	explicit ImGuiMaterial(Xenon::Instance& instance);

	/**
	 * Explicit constructor.
	 *
	 * @param instance The instance reference.
	 * @param pImage The image pointer to create the image with.
	 */
	explicit ImGuiMaterial(Xenon::Instance& instance, const Xenon::Backend::Image* pImage);

	/**
	 * Get the rasterizing pipeline specification.
	 *
	 * @return The pipeline specification.
	 */
	[[nodiscard]] Xenon::Backend::RasterizingPipelineSpecification getRasterizingSpecification() override;

	/**
	 * Create the material descriptor for the current material.
	 *
	 * @param pPipeline The pipeline pointer to create the material descriptor from.
	 * @return The material pointer.
	 */
	[[nodiscard]] std::unique_ptr<Xenon::Backend::Descriptor> createDescriptor(Xenon::Backend::Pipeline* pPipeline) override;

	/**
	 * Perform an image copy and copy the source image to the material's image.
	 *
	 * @param pCommandRecorder The command recorder pointer.
	 * @param pImage The image to copy the data from.
	 */
	void performCopy(Xenon::Backend::CommandRecorder* pCommandRecorder, Xenon::Backend::Image* pImage) const;

private:
	Xenon::Backend::RasterizingPipelineSpecification m_RasterizingPipelineSpecification = {};

	std::unique_ptr<Xenon::Backend::Image> m_pImage = nullptr;
	std::unique_ptr<Xenon::Backend::ImageView> m_pImageView = nullptr;
	std::unique_ptr<Xenon::Backend::ImageSampler> m_pSampler = nullptr;
};