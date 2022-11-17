// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Mutex.hpp"
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
			/**
			 * Queue type enum.
			 */
			enum class QueueType : uint8_t
			{
				Compute,
				Graphics,
				Transfer
			};

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
			 * Convert the Xenon multi sampling count to the Vulkan multi sampling count bits.
			 *
			 * @param count The Xenon multi sampling count.
			 * @return The Vulkan multi sampling count.
			 */
			[[nodiscard]] VkSampleCountFlagBits convertSamplingCount(MultiSamplingCount count) const;

			/**
			 * Convert the Xenon data format to the Vulkan format.
			 *
			 * @param format The data format.
			 * @return The Vulkan format.
			 */
			[[nodiscard]] VkFormat convertFormat(DataFormat format) const;

		public:
			/**
			 * Get the instance pointer to which the object is bound to.
			 *
			 * @return The instance pointer.
			 */
			[[nodiscard]] VulkanInstance* getInstance() { return m_pInstance; }

			/**
			 * Get the instance pointer to which the object is bound to.
			 *
			 * @return The const instance pointer.
			 */
			[[nodiscard]] const VulkanInstance* getInstance() const { return m_pInstance; }

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
			[[nodiscard]] Mutex<VulkanQueue>& getComputeQueue();

			/**
			 * Get the compute queue from the device.
			 *
			 * @return The compute queue.
			 */
			[[nodiscard]] const Mutex<VulkanQueue>& getComputeQueue() const;

			/**
			 * Get the graphics queue from the device.
			 *
			 * @return The graphics queue.
			 */
			[[nodiscard]] Mutex<VulkanQueue>& getGraphicsQueue();

			/**
			 * Get the graphics queue from the device.
			 *
			 * @return The graphics queue.
			 */
			[[nodiscard]] const Mutex<VulkanQueue>& getGraphicsQueue() const;

			/**
			 * Get the transfer queue from the device.
			 *
			 * @return The transfer queue.
			 */
			[[nodiscard]] Mutex<VulkanQueue>& getTransferQueue();

			/**
			 * Get the transfer queue from the device.
			 *
			 * @return The transfer queue.
			 */
			[[nodiscard]] const Mutex<VulkanQueue>& getTransferQueue() const;

			/**
			 * Get the compute command pool.
			 *
			 * @return The compute command pool.
			 */
			[[nodiscard]] Mutex<VkCommandPool>& getComputeCommandPool() { return m_ComputeCommandPool; }

			/**
			 * Get the compute command pool.
			 *
			 * @return The compute command pool.
			 */
			[[nodiscard]] const Mutex<VkCommandPool>& getComputeCommandPool() const { return m_ComputeCommandPool; }

			/**
			 * Get the graphics command pool.
			 *
			 * @return The graphics command pool.
			 */
			[[nodiscard]] Mutex<VkCommandPool>& getGraphicsCommandPool() { return m_GraphicsCommandPool; }

			/**
			 * Get the graphics command pool.
			 *
			 * @return The graphics command pool.
			 */
			[[nodiscard]] const Mutex<VkCommandPool>& getGraphicsCommandPool() const { return m_GraphicsCommandPool; }

			/**
			 * Get the transfer command pool.
			 *
			 * @return The transfer command pool.
			 */
			[[nodiscard]] Mutex<VkCommandPool>& getTransferCommandPool() { return m_TransferCommandPool; }

			/**
			 * Get the transfer command pool.
			 *
			 * @return The transfer command pool.
			 */
			[[nodiscard]] const Mutex<VkCommandPool>& getTransferCommandPool() const { return m_TransferCommandPool; }

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

			std::vector<Mutex<VulkanQueue>> m_Queues;

			std::vector<const char*> m_DeviceExtensions;

			VulkanInstance* m_pInstance = nullptr;

			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

			Mutex<VkCommandPool> m_ComputeCommandPool = VK_NULL_HANDLE;
			Mutex<VkCommandPool> m_GraphicsCommandPool = VK_NULL_HANDLE;
			Mutex<VkCommandPool> m_TransferCommandPool = VK_NULL_HANDLE;

			VmaAllocator m_Allocator = nullptr;

			uint8_t m_ComputeQueueIndex = 0;
			uint8_t m_GraphicsQueueIndex = 0;
			uint8_t m_TransferQueueIndex = 0;
		};
	}
}