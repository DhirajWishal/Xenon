// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

#include "../XenonBackend/CommandBuffers.hpp"

namespace Xenon
{
	/**
	 * Command recorder class.
	 * This class is used to record commands from the engine and sends them to the device to be executed.
	 */
	class CommandRecorder final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param bufferCount The number of primitive buffers to contain.
		 */
		explicit CommandRecorder(Instance& instance, uint32_t bufferCount);

		/**
		 * Destructor.
		 */
		~CommandRecorder() = default;

	private:
		std::unique_ptr<Backend::CommandBuffers> m_pCommandBuffers = nullptr;
	};
}