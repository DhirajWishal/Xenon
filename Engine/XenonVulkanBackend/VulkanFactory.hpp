// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/IFactory.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan factory class.
		 * This is used to create Vulkan backend objects and is used by the abstraction layer and the frontend.
		 */
		class VulkanFactory final : public IFactory
		{
		public:
			/**
			 * Default constructor.
			 */
			VulkanFactory() = default;

			/**
			 * Default destructor.
			 */
			~VulkanFactory() override = default;

			/**
			 * Create a new instance.
			 *
			 * @param appliationName The name of the application.
			 * @param applicationVersion The application version.
			 * @return The instance pointer.
			 */
			[[nodiscard]] std::unique_ptr<Instance> createInstance(const std::string& applicationName, uint32_t applicationVersion) override;

			/**
			 * Create a new device.
			 *
			 * @param pInstance The instance pointer to which the device is bound to.
			 * @param requriedRenderTargets The render targets which are required to have.
			 * @return The device pointer.
			 */
			[[nodiscard]] std::unique_ptr<Device> createDevice(Instance* pInstance, RenderTargetType requiredRenderTargets) override;

			/**
			 * Create a new command recorder.
			 *
			 * @param pDevice The device pointer.
			 * @param usage The command recorder usage.
			 * @param bufferCount The backend primitive buffer count. Default is 1.
			 * @return The command recorder pointer.
			 */
			[[nodiscard]] std::unique_ptr<CommandRecorder> createCommandRecorder(Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount = 1) override;

			/**
			 * Create a new index buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param type The buffer type.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] std::unique_ptr<Buffer> createBuffer(Device* pDevice, uint64_t size, BufferType type) override;
		};
	}
}