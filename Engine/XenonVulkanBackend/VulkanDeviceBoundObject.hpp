// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan device bound object.
		 * This is the base class for all the Vulkan backend objects which is bound to the Vulkan device.
		 */
		class VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit VulkanDeviceBoundObject(VulkanDevice* pDevice) : m_pDevice(pDevice) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~VulkanDeviceBoundObject() = default;

			/**
			 * Get the Vulkan device pointer.
			 *
			 * @return The device pointer.
			 */
			[[nodiscard]] VulkanDevice* getDevice() { return m_pDevice; }

			/**
			 * Get the Vulkan device pointer.
			 *
			 * @return The const device pointer.
			 */
			[[nodiscard]] const VulkanDevice* getDevice() const { return m_pDevice; }

		protected:
			VulkanDevice* m_pDevice = nullptr;
		};
	}
}