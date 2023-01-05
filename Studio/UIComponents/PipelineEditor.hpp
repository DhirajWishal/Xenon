// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../UIComponent.hpp"

/**
 * Pipeline builder type.
 */
enum class PipelineBuilderType : uint8_t
{
	Rasterizing,
	RayTracing,
	Compute
};

/**
 * Pipeline builder class.
 * This class contains the necessary means to create a new pipeline.
 */
class PipelineBuilder final
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param type The builder type.
	 */
	explicit PipelineBuilder(PipelineBuilderType type) : m_Type(type) {}

private:
	PipelineBuilderType m_Type = PipelineBuilderType::Rasterizing;
};

/**
 * Pipeline editor class.
 * This contains a node graph and other information about a pipeline which can be edited.
 */
class PipeilneEditor final : public UIComponent
{
public:
	/**
	 * Default constructor.
	 */
	PipeilneEditor() = default;

	/**
	 * Begin the component draw.
	 *
	 * @delta The time difference between the previous frame and the current frame in nanoseconds.
	 */
	void begin(std::chrono::nanoseconds delta) override;

	/**
	 * End the component draw.
	 */
	void end() override;

private:
	/**
	 * Show all the nodes.
	 */
	void showNodes();
};