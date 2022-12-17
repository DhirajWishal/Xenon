// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanImageView.hpp"
#include "VulkanMacros.hpp"

namespace /* anonymous */
{
	/**
	 * Get the component swizzle.
	 *
	 * @param swizzle The component swizzle.
	 * @return The Vulkan component swizzle.
	 */
	[[nodiscard]] constexpr VkComponentSwizzle GetComponentSwizzle(Xenon::Backend::ComponentSwizzle swizzle) noexcept
	{
		switch (swizzle)
		{
		case Xenon::Backend::ComponentSwizzle::Identity:
			return VK_COMPONENT_SWIZZLE_IDENTITY;

		case Xenon::Backend::ComponentSwizzle::Zero:
			return VK_COMPONENT_SWIZZLE_ZERO;

		case Xenon::Backend::ComponentSwizzle::One:
			return VK_COMPONENT_SWIZZLE_ONE;

		case Xenon::Backend::ComponentSwizzle::R:
			return VK_COMPONENT_SWIZZLE_R;

		case Xenon::Backend::ComponentSwizzle::G:
			return VK_COMPONENT_SWIZZLE_G;

		case Xenon::Backend::ComponentSwizzle::B:
			return VK_COMPONENT_SWIZZLE_B;

		case Xenon::Backend::ComponentSwizzle::A:
			return VK_COMPONENT_SWIZZLE_A;

		default:
			XENON_LOG_ERROR("Invalid component swizzle provided!");
			return VK_COMPONENT_SWIZZLE_IDENTITY;
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanImageView::VulkanImageView(VulkanDevice* pDevice, VulkanImage* pImage, const ImageViewSpecification& specification)
			: ImageView(pDevice, pImage, specification)
			, VulkanDeviceBoundObject(pDevice)
		{
			VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
			switch (pImage->getSpecification().m_Type)
			{
			case Xenon::Backend::ImageType::OneDimensional:
				viewType = VK_IMAGE_VIEW_TYPE_2D;
				break;

			case Xenon::Backend::ImageType::TwoDimensional:
				break;

			case Xenon::Backend::ImageType::CubeMap:
				viewType = VK_IMAGE_VIEW_TYPE_CUBE;
				break;

			case Xenon::Backend::ImageType::ThreeDimensional:
				viewType = VK_IMAGE_VIEW_TYPE_3D;
				break;

			default:
				XENON_LOG_ERROR("Invalid image type! Defaulting to TwoDimensional.");
				break;
			}

			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.image = pImage->getImage();
			createInfo.viewType = viewType;
			createInfo.format = pDevice->convertFormat(pImage->getDataFormat());
			createInfo.components.r = GetComponentSwizzle(specification.m_ComponentR);
			createInfo.components.g = GetComponentSwizzle(specification.m_ComponentG);
			createInfo.components.b = GetComponentSwizzle(specification.m_ComponentB);
			createInfo.components.a = GetComponentSwizzle(specification.m_ComponentA);
			createInfo.subresourceRange.aspectMask = pImage->getAspectFlags();
			createInfo.subresourceRange.baseMipLevel = specification.m_BaseMipLevel;
			createInfo.subresourceRange.levelCount = specification.m_LevelCount;
			createInfo.subresourceRange.baseArrayLayer = specification.m_BaseArrayLayer;
			createInfo.subresourceRange.layerCount = specification.m_LayerCount;

			XENON_VK_ASSERT(pDevice->getDeviceTable().vkCreateImageView(pDevice->getLogicalDevice(), &createInfo, nullptr, &m_View), "Failed to create the image view!");
		}

		VulkanImageView::~VulkanImageView()
		{
			m_pDevice->getDeviceTable().vkDestroyImageView(m_pDevice->getLogicalDevice(), m_View, nullptr);
		}
	}
}