// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBuffers.hpp"
#include "IndexBuffer.hpp"
#include "StagingBuffer.hpp"
#include "StorageBuffer.hpp"
#include "UniformBuffer.hpp"
#include "VertexBuffer.hpp"

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
			 * Create new command buffers.
			 *
			 * @param pDevice The device pointer.
			 * @param bufferCount The number of buffers to use.
			 * @return The command buffers pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<CommandBuffers> createCommandBuffers(Device* pDevice, uint32_t bufferCount) = 0;

			/**
			 * Create a new index buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param indexSize The size of a single index.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<IndexBuffer> createIndexBuffer(Device* pDevice, uint64_t size, IndexSize indexSize) = 0;

			/**
			 * Create a new vertex buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param stride The vertex stride.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<VertexBuffer> createVertexBuffer(Device* pDevice, uint64_t size, uint64_t stride) = 0;

			/**
			 * Create a new staging buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<StagingBuffer> createStagingBuffer(Device* pDevice, uint64_t size) = 0;

			/**
			 * Create a new storage buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<StorageBuffer> createStorageBuffer(Device* pDevice, uint64_t size) = 0;

			/**
			 * Create a new uniform buffer.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @return The buffer pointer.
			 */
			[[nodiscard]] virtual std::unique_ptr<UniformBuffer> createUniformBuffer(Device* pDevice, uint64_t size) = 0;
		};
	}
}