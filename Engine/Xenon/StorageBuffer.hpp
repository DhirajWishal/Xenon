// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

#include "../XenonBackend/StorageBuffer.hpp"

namespace Xenon
{
	/**
	 * Staging buffer class.
	 * Staging buffers are used to store data temporarily and to be transferred to other containers.
	 */
	class StorageBuffer final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param size The size of the buffer.
		 */
		explicit StorageBuffer(Instance& instance, uint64_t size);

		/**
		 * Default virtual destructor.
		 */
		~StorageBuffer() = default;

	private:
		std::unique_ptr<Backend::StorageBuffer> m_pStorageBuffer = nullptr;
	};
}