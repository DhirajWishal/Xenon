// Copyright 2022-2023 Nexonous
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
		class VulkanDescriptorSetManager;

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

			/**
			 * Finish all device operations and wait idle.
			 */
			void waitIdle() override;

		public:
			/**
			 * Convert the Xenon multi sampling count to the Vulkan multi sampling count bits.
			 *
			 * @param count The Xenon multi sampling count.
			 * @return The Vulkan multi sampling count.
			 */
			[[nodiscard]] static VkSampleCountFlagBits ConvertSamplingCount(MultiSamplingCount count) noexcept;

			/**
			 * Convert the Xenon data format to the Vulkan format.
			 *
			 * @param format The data format.
			 * @return The Vulkan format.
			 */
			[[nodiscard]] static VkFormat ConvertFormat(DataFormat format) noexcept;

			/**
			 * Convert the Xenon resource type to the Vulkan descriptor type.
			 *
			 * @param type The resource type.
			 * @return The descriptor type.
			 */
			[[nodiscard]] static VkDescriptorType ConvertResourceType(ResourceType type) noexcept;

			/**
			 * Get the shader stage flag bit from the Xenon shader type.
			 *
			 * @param shaderType The shader type.
			 * @return The Vulkan shader stage flag bit.
			 */
			[[nodiscard]] static VkShaderStageFlagBits GetShaderStageFlagBit(Xenon::Backend::ShaderType shaderType) noexcept;

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
			[[nodiscard]] Mutex<VmaAllocator>& getAllocator() { return m_Allocator; }

			/**
			 * Get the VMA allocator.
			 *
			 * @return The allocator.
			 */
			[[nodiscard]] const Mutex<VmaAllocator>& getAllocator() const { return m_Allocator; }

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
			 * Get the physical device ray tracing pipeline properties.
			 *
			 * @return The properties.
			 */
			[[nodiscard]] const VkPhysicalDeviceRayTracingPipelinePropertiesKHR& getPhysicalDeviceRayTracingPipelineProperties() const { return m_RayTracingPipelineProperties; }

			/**
			 * Get the physical device acceleration structure features.
			 *
			 * @return The features.
			 */
			[[nodiscard]] const VkPhysicalDeviceAccelerationStructureFeaturesKHR& getPhysicalDeviceAccelerationStructureProperties() const { return m_AccelerationStructureFeatures; }

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
			 * Get the descriptor set manager.
			 *
			 * @return The descriptor set manager pointer.
			 */
			[[nodiscard]] VulkanDescriptorSetManager* getDescriptorSetManager() { return m_pDescriptorSetManager; }

			/**
			 * Get the descriptor set manager.
			 *
			 * @return The descriptor set manager pointer.
			 */
			[[nodiscard]] const VulkanDescriptorSetManager* getDescriptorSetManager() const { return m_pDescriptorSetManager; }

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

		private:
			VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};
			VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_RayTracingPipelineProperties = {};
			VkPhysicalDeviceAccelerationStructureFeaturesKHR m_AccelerationStructureFeatures = {};

			VolkDeviceTable m_DeviceTable;

			std::vector<Mutex<VulkanQueue>> m_Queues;

			std::vector<const char*> m_DeviceExtensions;

			VulkanInstance* m_pInstance = nullptr;

			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

			Mutex<VmaAllocator> m_Allocator = nullptr;

			VulkanDescriptorSetManager* m_pDescriptorSetManager = nullptr;

			uint8_t m_ComputeQueueIndex = 0;
			uint8_t m_GraphicsQueueIndex = 0;
			uint8_t m_TransferQueueIndex = 0;
		};
	}
}