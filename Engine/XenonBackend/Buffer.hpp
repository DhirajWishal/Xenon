// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		class CommandBuffers;

		/**
		 * Buffer class.
		 * This is the base class for all the backend buffers used by Xenon.
		 */
		class Buffer : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 */
			explicit Buffer([[maybe_unused]] Device* pDevice, uint64_t size) : m_Size(size) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Buffer() = default;

			/**
			 * Copy data from another buffer to this buffer.
			 *
			 * @param pBuffer The buffer to copy the data from.
			 * @param size The size in bytes to copy.
			 * @param srcOffset The source buffer's offset. Default is 0.
			 * @param dstOffset The destination buffer's (this) offset. Default is 0.
			 */
			virtual void copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0) = 0;

			/**
			 * Copy data from another buffer to this buffer.
			 * This will attach the copy command to the attached command buffers.
			 *
			 * @param pCommandBuffers The command buffers to record the commands to.
			 * @param pBuffer The buffer to copy the data from.
			 * @param size The size in bytes to copy.
			 * @param srcOffset The source buffer's offset. Default is 0.
			 * @param dstOffset The destination buffer's (this) offset. Default is 0.
			 */
			virtual void copy(CommandBuffers* pCommandBuffers, const Buffer* pBuffer, uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0) = 0;

		protected:
			uint64_t m_Size = 0;
		};
	}
}