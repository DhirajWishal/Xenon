// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Device.hpp"

#include "VulkanInstance.hpp"
#include "VulkanQueue.hpp"

#include <vk_mem_alloc.h>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan device class.
		 */
		class VulkanDevice final : public Device
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pInstance The Vulkan instance pointer.
			 * @param requiredRenderTargets The required render targets.
			 */
			explicit VulkanDevice(VulkanInstance* pInstance, RenderTargetType requiredRenderTargets);

			/**
			 * Destructor.
			 */
			~VulkanDevice() override;

		public:
			/**
			 * Get the physical device.
			 *
			 * @return The physical device.
			 */
			[[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }

			/**
			 * Get the logical device.
			 *
			 * @return The logical device.
			 */
			[[nodiscard]] VkDevice getLogicalDevice() const { return m_LogicalDevice; }

			/**
			 * Get the VMA allocator.
			 *
			 * @return The allocator.
			 */
			[[nodiscard]] VmaAllocator getAllocator() const { return m_Allocator; }

			/**
			 * Get the device table.
			 *
			 * @return The device table const reference.
			 */
			[[nodiscard]] const VolkDeviceTable& getDeviceTable() const { return m_DeviceTable; }

			/**
			 * Get the physical device properties.
			 *
			 * @return The physical device properties.
			 */
			[[nodiscard]] const VkPhysicalDeviceProperties& getPhysicalDeviceProperties() const { return m_PhysicalDeviceProperties; }

			/**
			 * Get the compute queue from the device.
			 *
			 * @return The compute queue.
			 */
			[[nodiscard]] const VulkanQueue& getComputeQueue() const { return m_ComputeQueue; }

			/**
			 * Get the graphics queue from the device.
			 *
			 * @return The graphics queue.
			 */
			[[nodiscard]] const VulkanQueue& getGraphicsQueue() const { return m_GraphicsQueue; }

			/**
			 * Get the transfer queue from the device.
			 *
			 * @return The transfer queue.
			 */
			[[nodiscard]] const VulkanQueue& getTransferQueue() const { return m_TransferQueue; }

			/**
			 * Get the compute command pool.
			 *
			 * @return The compute command pool.
			 */
			[[nodiscard]] VkCommandPool getComputeCommandPool() const { return m_ComputeCommandPool; }

			/**
			 * Get the graphics command pool.
			 *
			 * @return The graphics command pool.
			 */
			[[nodiscard]] VkCommandPool getGraphicsCommandPool() const { return m_GraphicsCommandPool; }

			/**
			 * Get the transfer command pool.
			 *
			 * @return The transfer command pool.
			 */
			[[nodiscard]] VkCommandPool getTransferCommandPool() const { return m_TransferCommandPool; }

		private:
			/**
			 * Select the required physical device.
			 */
			void selectPhysicalDevice();

			/**
			 * Create the logical device.
			 */
			void createLogicalDevice();

			/**
			 * Create the VMA allocator.
			 */
			void createMemoryAllocator();

			/**
			 * Create the compute, graphics and transfer command pools.
			 */
			void createCommandPools();

		private:
			VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};
			VolkDeviceTable m_DeviceTable;

			VulkanQueue m_ComputeQueue;
			VulkanQueue m_GraphicsQueue;
			VulkanQueue m_TransferQueue;

			std::vector<const char*> m_DeviceExtensions;

			VulkanInstance* m_pInstance = nullptr;

			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

			VkCommandPool m_ComputeCommandPool = VK_NULL_HANDLE;
			VkCommandPool m_GraphicsCommandPool = VK_NULL_HANDLE;
			VkCommandPool m_TransferCommandPool = VK_NULL_HANDLE;

			VmaAllocator m_Allocator = nullptr;
		};
	}
}