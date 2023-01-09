// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.hpp"

namespace Xenon
{
	/**
	 * Geometry store class.
	 * This class is used to create and store geometries.
	 */
	class GeometryStore final : public XObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 */
		explicit GeometryStore(Instance& instance) : m_Instance(instance) {}

		/**
		 * Create a new geometry by loading it from a file.
		 * This is done asynchronously so use the returned future to get notified when completed, or to wait till the geometry is loaded.
		 *
		 * @param path The file path.
		 * @return The loaded geometry reference.
		 */
		std::future<Geometry&> create(const std::filesystem::path& path);

	private:
		Instance& m_Instance;

		std::vector<Geometry> m_Geometries;
	};
}