// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "HostAccessibleBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Staging buffer class.
		 * This class is used to temporarily store data and is used for transferring purposes.
		 */
		class StagingBuffer : public HostAccessibleBuffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 */
			explicit StagingBuffer(Device* pDevice, uint64_t size) : HostAccessibleBuffer(pDevice, size) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~StagingBuffer() = default;
		};
	}
}