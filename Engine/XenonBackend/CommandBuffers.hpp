// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		class Buffer;

		/**
		 * Backend object class.
		 * This object is used to record commands and transfer it to the device to be executed.
		 */
		class CommandBuffers : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param bufferCount The number of primitive buffers.
			 */
			explicit CommandBuffers([[maybe_unused]] Device* pDevice, uint32_t bufferCount) : m_BufferCount(bufferCount) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~CommandBuffers() = default;

			/**
			 * This will internally select the next command buffer primitive for recording and execution.
			 */
			virtual void next() = 0;

			/**
			 * Submit all the graphics commands.
			 *
			 * @param shouldWait Whether we should wait till the commands are executed. Default is true.
			 */
			virtual void submitGraphics(bool shouldWait = true) = 0;

			/**
			 * Submit all the compute commands.
			 *
			 * @param shouldWait Whether we should wait till the commands are executed. Default is true.
			 */
			virtual void submitCompute(bool shouldWait = true) = 0;

			/**
			 * Submit all the transfer commands.
			 *
			 * @param shouldWait Whether we should wait till the commands are executed. Default is true.
			 */
			virtual void submitTransfer(bool shouldWait = true) = 0;

		public:
			/**
			 * Begin the current command buffer recording.
			 */
			virtual void begin() = 0;

			/**
			 * Copy data from one buffer to another.
			 *
			 * @param pSourceBuffer The source buffer to copy data from.
			 * @param srcOffset The source buffer's offset to copy from.
			 * @param pDestinationBuffer The destination buffer to copy the data to.
			 * @param dstOffset The destination buffer's offset to copy the data to.
			 * @param size The number of bytes to copy.
			 */
			virtual void copyBuffers(const Buffer* pSourceBuffer, uint64_t srcOffset, const Buffer* pDestinationBuffer, uint64_t dstOffset, uint64_t size) = 0;

			/**
			 * End the current command buffer recording.
			 */
			virtual void end() = 0;

		protected:
			const uint32_t m_BufferCount;
		};
	}
}