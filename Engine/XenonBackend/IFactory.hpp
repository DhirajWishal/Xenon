// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandRecorder.hpp"
#include "Buffer.hpp"
#include "Image.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Interface factory class.
		 * This factory creates backend classes from a single backend.
		 */
		class IFactory
		{
		public:
			/**
			 * Default constructor.
			 */
			IFactory() = default;

			/**
			 * Default virtual constructor.
			 */
			virtual ~IFactory() = default;

			/**
			 * Create a new instance.
			 *
			 * @param appliationName The name of the application.
			 * @param applicationVersion The application version.
			 * @return The instance pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Instance> createInstance(const std::string& applicationName, uint32_t applicationVersion) = 0;

			/**
			 * Create a new device.
			 *
			 * @param pInstance The instance pointer to which the device is bound to.
			 * @param requriedRenderTargets The render targets which are required to have.
			 * @return The device pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Device> createDevice(Instance* pInstance, RenderTargetType requiredRenderTargets) = 0;

			/**
			 * Create a new command recorder.
			 *
			 * @param pDevice The device pointer.
			 * @param usage The command recorder usage.
			 * @param bufferCount The backend primitive buffer count. Default is 1.
			 * @return The command recorder pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<CommandRecorder> createCommandRecorder(Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount = 1) = 0;

			/**
			 * Create a new index buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param type The buffer type.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Buffer> createBuffer(Device* pDevice, uint64_t size, BufferType type) = 0;

			/**
			 * Create a new image.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The image specification.
			 * @return The image pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<Image> createImage(Device* pDevice, const ImageSpecification& specification) = 0;
		};
	}
}