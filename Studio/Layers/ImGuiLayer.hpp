// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Xenon/RasterizingLayer.hpp"

#include "XenonCore/SparseArray.hpp"
#include "XenonCore/Logging.hpp"
#include "XenonBackend/Buffer.hpp"
#include "XenonBackend/RasterizingPipeline.hpp"

#include "../UIComponents/LayerView.hpp"
#include "../UIComponents/PerformanceMetrics.hpp"
#include "../UIComponents/PipelineEditor.hpp"
#include "../UIComponents/Configuration.hpp"
#include "../UIComponents/Logs.hpp"

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

	/**
	 * UI storage structure.
	 * This structure contains all the UI components which are used by the studio.
	 * This layer also manages them.
	 */
	struct UIStorage final
	{
		explicit UIStorage(ImGuiLayer* pLayer) : m_LayerViewUI(pLayer), m_ConfigurationUI(pLayer), m_pLogs(std::make_shared<Logs>()) {}

		LayerView m_LayerViewUI;
		Configuration m_ConfigurationUI;
		PerformanceMetrics m_PerformanceMetricsUI;
		PipeilneEditor m_PipelineEditorUI;

		std::shared_ptr<Logs> m_pLogs;
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
	 *
	 * @param delta The time difference between the previous frame and the current frame in nanoseconds.
	 * @return True if the user should handle the inputs.
	 * @return False if the events are handled internally and the user should not handle inputs.
	 */
	[[nodiscard]] bool beginFrame(std::chrono::nanoseconds delta);

	/**
	 * End the frame.
	 * This function must be called to render things on the screen!
	 */
	void endFrame() const;

	/**
	 * Update the layer.
	 * This is called by the renderer and all the required commands must be updated (if required) in this call.
	 *
	 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
	 * @param imageIndex The image's index.
	 * @param frameIndex The frame's index.
	 */
	void onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex) override;

	/**
	 * Register a layer to be shown.
	 *
	 * @param pLayer The layer to be shown.
	 */
	void showLayer(Xenon::Layer* pLayer);

	/**
	 * Set the draw call count.
	 *
	 * @param totalCount The total draw call count.
	 * @param actualCount The actual draw call count.
	 */
	void setDrawCallCount(uint64_t totalCount, uint64_t actualCount);

	/**
	 * Get the image ID for an image that is to be rendered using the ImGui layer.
	 *
	 * @param pImage The image pointer.
	 * @param pImageView The image view pointer.
	 * @param pImageSampler The image sampler pointer.
	 * @return The texture ID.
	 */
	[[nodiscard]] uintptr_t getImageID(Xenon::Backend::Image* pImage, Xenon::Backend::ImageView* pImageView, Xenon::Backend::ImageSampler* pImageSampler);

private:
	/**
	 * Configure ImGui.
	 */
	void configureImGui() const;

	/**
	 * Setup the default texture material.
	 */
	void setupDefaultMaterial();

	/**
	 * Setup the pipeline.
	 */
	void setupPipeline();

	/**
	 * Prepare the resources to render.
	 *
	 * @param pCommandRecorder The command recorder to record the copy commands to.
	 * @param frameIndex The current frame index.
	 */
	void prepareResources(Xenon::Backend::CommandRecorder* pCommandRecorder, uint32_t frameIndex);

	/**
	 * Calculate the next buffer size with some overhead.
	 *
	 * @param requiredSize The size required by the buffer.
	 * @return The size requested to be allocated.
	 */
	[[nodiscard]] uint64_t getNextBufferSize(uint64_t requiredSize) const;

	/**
	 * Show the main menu.
	 */
	void showMainMenu();

	/**
	 * Show the file menu.
	 */
	void showFileMenu();

	/**
	 * Show the edit menu.
	 */
	void showEditMenu();

	/**
	 * Show the view menu.
	 */
	void showViewMenu();

	/**
	 * Show the help menu.
	 */
	void showHelpMenu();

	/**
	 * Show all the UIs.
	 *
	 * @param delta The time taken from the previous frame to this.
	 */
	void showUIs(std::chrono::nanoseconds delta);

private:
	UIStorage m_UIStorage;

	std::unique_ptr<Xenon::Backend::RasterizingPipeline> m_pPipeline = nullptr;

	std::unordered_map<uintptr_t, std::unique_ptr<Xenon::Backend::Descriptor>> m_pMaterialDescriptors;

	std::unique_ptr<Xenon::Backend::Descriptor> m_pUserDescriptor = nullptr;
	std::unique_ptr<Xenon::Backend::Buffer> m_pUniformBuffer = nullptr;

	std::shared_ptr<spdlog::logger> m_pDefaultLogger = nullptr;

	std::unique_ptr<Xenon::Backend::Image> m_pImage = nullptr;
	std::unique_ptr<Xenon::Backend::ImageView> m_pImageView = nullptr;
	std::unique_ptr<Xenon::Backend::ImageSampler> m_pSampler = nullptr;

	std::vector<std::unique_ptr<Xenon::Backend::Buffer>> m_pVertexBuffers;
	std::vector<std::unique_ptr<Xenon::Backend::Buffer>> m_pIndexBuffers;

	std::vector<Xenon::Backend::Rasterizer::ClearValueType> m_ClearValues;

	Xenon::Backend::VertexSpecification m_VertexSpecification;

	UserData m_UserData;
};