// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VertexBuffer.hpp"
#include "VertexTraits.hpp"

namespace Xenon
{
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

	private:
		VertexSpecification m_VertexSpecification;
		std::unique_ptr<Backend::VertexBuffer> m_pVertexBuffer = nullptr;
	};
}