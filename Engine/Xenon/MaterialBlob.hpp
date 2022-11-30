// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/XObject.hpp"
#include "../XenonBackend/RasterizingPipeline.hpp"

namespace Xenon
{
	class Instance;

	/**
	 * Material blob class.
	 * This is the base class for all the materials of a single sub-mesh.
	 */
	class MaterialBlob : public XObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 */
		explicit MaterialBlob(Instance& instance) : m_Instance(instance) {}

		/**
		 * Get the rasterizing pipeline specification.
		 *
		 * @return The pipeline specification.
		 */
		[[nodiscard]] virtual Backend::RasterizingPipelineSpecification getRasterizingSpecification() = 0;

		/**
		 * Create the material descriptor for the current material.
		 *
		 * @param pPipeline The pipeline pointer to create the material descriptor from.
		 * @return The material pointer.
		 */
		[[nodiscard]] virtual std::unique_ptr<Backend::Descriptor> createDescriptor(Backend::Pipeline* pPipeline) = 0;

	protected:
		Instance& m_Instance;
	};
}