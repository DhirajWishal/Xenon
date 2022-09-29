// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

#include "../XenonBackend/UniformBuffer.hpp"

namespace Xenon
{
	/**
	 * Uniform buffer class.
	 * These buffers are used to store information which will be passed to the shaders when drawing.
	 */
	class UniformBuffer final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param size The size of the buffer.
		 */
		explicit UniformBuffer(Instance& instance, uint64_t size);

		/**
		 * Default virtual destructor.
		 */
		~UniformBuffer() = default;

	private:
		std::unique_ptr<Backend::UniformBuffer> m_pUniformBuffer = nullptr;
	};
}