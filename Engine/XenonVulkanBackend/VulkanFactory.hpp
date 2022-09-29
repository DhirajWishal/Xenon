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
			 * Create new command buffers.
			 *
			 * @param pDevice The device pointer.
			 * @param bufferCount The number of buffers to use.
			 */
			[[nodiscard]] std::unique_ptr<CommandBuffers> createCommandBuffers(Device* pDevice, uint32_t bufferCount) override;

			/**
			 * Create a new index buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param indexSize The size of a single index.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] std::unique_ptr<IndexBuffer> createIndexBuffer(Device* pDevice, uint64_t size, IndexSize indexSize) override;

			/**
			 * Create a new vertex buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param stride The vertex stride.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] std::unique_ptr<VertexBuffer> createVertexBuffer(Device* pDevice, uint64_t size, uint64_t stride) override;

			/**
			 * Create a new staging buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] std::unique_ptr<StagingBuffer> createStagingBuffer(Device* pDevice, uint64_t size) override;

			/**
			 * Create a new storage buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] std::unique_ptr<StorageBuffer> createStorageBuffer(Device* pDevice, uint64_t size) override;

			/**
			 * Create a new uniform buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] std::unique_ptr<UniformBuffer> createUniformBuffer(Device* pDevice, uint64_t size) override;
		};
	}
}