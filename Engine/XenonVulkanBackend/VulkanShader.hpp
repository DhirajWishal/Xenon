// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/ShaderSource.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan shader class.
		 * This class contains the Vulkan specific shader module and other information.
		 */
		class VulkanShader final : public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param source The shader source.
			 * @param stageFlags The Vulkan shader stage flags.
			 */
			explicit VulkanShader(VulkanDevice* pDevice, const ShaderSource& source, VkShaderStageFlags stageFlags);

			/**
			 * Destructor.
			 */
			~VulkanShader() override;

			/**
			 * Get the shader module.
			 *
			 * @return The shader module.
			 */
			[[nodiscard]] VkShaderModule getModule() const { return m_ShaderModule; }

			/**
			 * Get the shader stage flags.
			 *
			 * @return The shader stage flags of the shader.
			 */
			[[nodiscard]] VkShaderStageFlags getStageFlags() const { return m_Flags; }

		private:
			VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
			VkShaderStageFlags m_Flags = 0;
		};
	}
}