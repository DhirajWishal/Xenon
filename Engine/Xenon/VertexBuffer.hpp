// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

#include "../XenonBackend/VertexBuffer.hpp"

namespace Xenon
{
	/**
	 * Vertex buffer class.
	 * This class stores vertex information which is used by the command buffers to draw geometry from.
	 */
	class VertexBuffer final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param size The size of the buffer.
		 * @param stride The vertex stride.
		 */
		explicit VertexBuffer(Instance& instance, uint64_t size, uint64_t stride);

		/**
		 * Default virtual destructor.
		 */
		~VertexBuffer() = default;

	private:
		std::unique_ptr<Backend::VertexBuffer> m_pVertexBuffer = nullptr;
	};
}