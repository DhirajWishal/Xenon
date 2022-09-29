// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

#include "../XenonBackend/IndexBuffer.hpp"

namespace Xenon
{
	/**
	 * Index buffer class.
	 * This class is used to store index data which is then provided to the command buffers to be drawn from.
	 */
	class IndexBuffer final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param size The size of the buffer.
		 * @param indexSize The size of a single index.
		 */
		explicit IndexBuffer(Instance& instance, uint64_t size, IndexSize indexSize);

		/**
		 * Default virtual destructor.
		 */
		~IndexBuffer() = default;

	private:
		std::unique_ptr<Backend::IndexBuffer> m_pIndexBuffer = nullptr;
	};
}