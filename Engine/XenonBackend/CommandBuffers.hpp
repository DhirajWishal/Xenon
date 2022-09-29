// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
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

		protected:
			const uint32_t m_BufferCount;
		};
	}
}