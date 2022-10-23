// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "VertexSpecification.hpp"

#include <filesystem>

namespace Xenon
{
	/**
	 * Mesh structure.
	 * This contains information about a single mesh instance.
	 */
	struct Mesh final
	{
		std::string m_Name;

		uint64_t m_IndexCount = 0;
		uint64_t m_IndexOffset = 0;

		uint64_t m_VertexCount = 0;
		uint64_t m_VertexOffset = 0;
	};

	/**
	 * Mesh storage class.
	 * This class contains all the meshes of a single model, with or without animation.
	 *
	 * All meshes are stored in a tree like hierarchy. But for performance, all the mesh data (vertex data and index data) are stored in individual vertex and index buffers, and
	 * mesh specific information (offsets, names, materials, etc...) are stored in a vector where each mesh contains information about child nodes and so on.
	 */
	class MeshStorage final
	{
	public:
		/**
		 * Explicit constructor.
		 */
		explicit MeshStorage(Instance& instance) {}

		/**
		 * Load the meshes from a file and create the mesh storage class.
		 *
		 * @param instance The instance reference.
		 * @param file The file path to load the data from.
		 * @return The created mesh storage.
		 */
		[[nodiscard]] static MeshStorage FromFile(Instance& instance, const std::filesystem::path& file);

	private:
		std::unique_ptr<Backend::Buffer> m_pIndexBuffer = nullptr;
		std::unique_ptr<Backend::Buffer> m_pVertexBuffer = nullptr;

		std::vector<Mesh> m_Meshes;

		VertexSpecification m_VertexSpecification;
	};
}