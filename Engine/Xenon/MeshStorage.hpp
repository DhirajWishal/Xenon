// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "VertexSpecification.hpp"

#include <filesystem>

namespace Xenon
{
	/**
	 * Primitive mode.
	 * This defines what the primitive mode is for a single
	 */
	enum class PrimitiveMode : uint8_t
	{
		Points,
		Line,
		LineLoop,
		LineStrip,
		Triangles,
		TriangleStrip,
		TriangleFan
	};

	/**
	 * Index size enum.
	 */
	enum class IndexSize : uint8_t
	{
		Unknown = 0,
		Uint8 = sizeof(uint8_t),
		Uint16 = sizeof(uint16_t),
		Uint32 = sizeof(uint32_t)
	};

	/**
	 * Sub-mesh structure.
	 * Sub-meshes are the building blocks of a mesh.
	 */
	struct SubMesh final
	{
		uint64_t m_VertexOffset = 0;
		uint64_t m_VertexCount = 0;

		uint64_t m_IndexOffset = 0;
		uint64_t m_IndexCount = 0;		// If this is set to 0, it will draw using the vertices.

		PrimitiveMode m_Mode = PrimitiveMode::Triangles;
		IndexSize m_IndexSize = IndexSize::Unknown;
	};

	/**
	 * Mesh structure.
	 * This contains information about a single mesh instance.
	 */
	struct Mesh final
	{
		std::string m_Name;
		std::vector<SubMesh> m_SubMeshes;
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
		 * Default constructor.
		 */
		MeshStorage() = default;

		/**
		 * Load the meshes from a file and create the mesh storage class.
		 *
		 * @param instance The instance reference.
		 * @param file The file path to load the data from.
		 * @return The created mesh storage.
		 */
		[[nodiscard]] static MeshStorage FromFile(Instance& instance, const std::filesystem::path& file);

		/**
		 * Get the vertex specification.
		 *
		 * @return The vertex specification.
		 */
		[[nodiscard]] const VertexSpecification& getVertexSpecification() const { return m_VertexSpecification; }

		/**
		 * Get the meshes.
		 *
		 * @return The meshes.
		 */
		[[nodiscard]] std::vector<Mesh>& getMeshes() { return m_Meshes; }

		/**
		 * Get the meshes.
		 *
		 * @return The meshes.
		 */
		[[nodiscard]] const std::vector<Mesh>& getMeshes() const { return m_Meshes; }

	private:
		std::unique_ptr<Backend::Buffer> m_pIndexBuffer = nullptr;
		std::unique_ptr<Backend::Buffer> m_pVertexBuffer = nullptr;

		std::vector<Mesh> m_Meshes;

		VertexSpecification m_VertexSpecification;
	};
}