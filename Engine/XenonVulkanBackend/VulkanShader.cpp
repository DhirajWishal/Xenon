// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanShader.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanShader::VulkanShader(VulkanDevice* pDevice, const ShaderSource& source, VkShaderStageFlags stageFlags)
			: VulkanDeviceBoundObject(pDevice)
			, m_Flags(stageFlags)
		{
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = stageFlags;
			createInfo.codeSize = source.getBinary().size();
			createInfo.pCode = source.getBinary().data();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateShaderModule(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_ShaderModule), "Failed to create the shader module!");
		}

		VulkanShader::~VulkanShader()
		{
			try
			{
				m_pDevice->getInstance()->getDeletionQueue().insert([pDevice = m_pDevice, shaderModule = m_ShaderModule]
					{
						pDevice->getDeviceTable().vkDestroyShaderModule(pDevice->getLogicalDevice(), shaderModule, nullptr);
					}
				);
			}
			catch (...)
			{
				XENON_VK_ASSERT(VK_ERROR_UNKNOWN, "Failed to push the shader deletion function to the deletion queue!");
			}
		}
	}
}