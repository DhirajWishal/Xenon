// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Xenon/RasterizingLayer.hpp"
#include "Xenon/MaterialBlob.hpp"

#include "XenonCore/TaskNode.hpp"
#include "XenonBackend/Buffer.hpp"
#include "XenonBackend/RasterizingPipeline.hpp"

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
	explicit ImGuiMaterial(Xenon::Instance& instance, Xenon::Backend::Image* pImage);

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

private:
	Xenon::Backend::RasterizingPipelineSpecification m_RasterizingPipelineSpecification = {};

	std::unique_ptr<Xenon::Backend::Image> m_pImage = nullptr;
	std::unique_ptr<Xenon::Backend::ImageView> m_pImageView = nullptr;
	std::unique_ptr<Xenon::Backend::ImageSampler> m_pSampler = nullptr;
};

/**
 * ImGui layer class.
 * This class can be used to render ImGui.
 */
class ImGuiLayer final : public Xenon::RasterizingLayer
{
	/**
	 * User data structure.
	 */
	struct UserData final
	{
		glm::vec2 m_Scale;
		glm::vec2 m_Translate;
	};

public:
	/**
	 * Explicit constructor.
	 *
	 * @param renderer The renderer reference.
	 * @param pCamera The camera pointer.
	 */
	explicit ImGuiLayer(Xenon::Renderer& renderer, Xenon::Backend::Camera* pCamera);

	/**
	 * Destructor.
	 */
	~ImGuiLayer() override;

	/**
	 * Begin the frame.
	 * This function must be called before drawing anything on ImGui!
	 */
	void beginFrame() const;

	/**
	 * End the frame.
	 * This function must be called to render things on the screen!
	 */
	void endFrame();

	/**
	 * Bind the layer to the command recorder.
	 * This is where all the required commands must be submitted to the command recorder to be executed by the GPU.
	 *
	 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
	 * @param pCommandRecorder The command recorder pointer to bind the commands to.
	 */
	void bind(Xenon::Layer* pPreviousLayer, Xenon::Backend::CommandRecorder* pCommandRecorder) override;

private:
	/**
	 * Configure ImGui.
	 */
	void configureImGui();

	/**
	 * Setup the default texture material.
	 */
	void setupDefaultMaterial();

	/**
	 * Prepare the resources to render.
	 *
	 * @param pCommandRecorder The command recorder to record the copy commands to.
	 */
	void prepareResources(Xenon::Backend::CommandRecorder* pCommandRecorder);

	/**
	 * Calculate the next buffer size with some overhead.
	 *
	 * @param requiredSize The size required by the buffer.
	 * @return The size requested to be allocated.
	 */
	[[nodiscard]] uint64_t getNextBufferSize(uint64_t requiredSize) const;

private:
	std::unordered_map<uint64_t, std::unique_ptr<Xenon::Backend::Descriptor>> m_pDescriptorSetMap;

	std::shared_ptr<Xenon::TaskNode> m_pTaskNode = nullptr;

	std::unique_ptr<Xenon::Backend::RasterizingPipeline> m_pPipeline = nullptr;
	std::unique_ptr<Xenon::Backend::Descriptor> m_pUserDescriptor = nullptr;

	std::vector<std::unique_ptr<Xenon::Backend::Buffer>> m_pVertexBuffers;
	std::vector<std::unique_ptr<Xenon::Backend::Buffer>> m_pIndexBuffers;

	std::unique_ptr<Xenon::Backend::Buffer> m_pUniformBuffer = nullptr;

	Xenon::Backend::VertexSpecification m_VertexSpecification;

	UserData m_UserData;

	Xenon::MaterialIdentifier m_DefaultMaterialIdentifier;
};