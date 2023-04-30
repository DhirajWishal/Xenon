// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "VulkanImageSampler.hpp"
#include "VulkanMacros.hpp"

namespace /* anonymous */
{
	/**
	 * Get the address mode.
	 *
	 * @param mode The xenon address mode.
	 * @return The Vulkan address mode.
	 */
	[[nodiscard]] constexpr VkSamplerAddressMode GetAddressMode(Xenon::Backend::AddressMode mode) noexcept
	{
		switch (mode)
		{
		case Xenon::Backend::AddressMode::Repeat:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;

		case Xenon::Backend::AddressMode::MirroredRepeat:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

		case Xenon::Backend::AddressMode::ClampToEdge:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		case Xenon::Backend::AddressMode::ClampToBorder:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

		case Xenon::Backend::AddressMode::MirrorClampToEdge:
			return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;

		default:
			XENON_LOG_ERROR("Invalid address mode!");
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}
	}

	/**
	 * Get the border color.
	 *
	 * @param color The xenon border color.
	 * @return The Vulkan border color.
	 */
	[[nodiscard]] constexpr VkBorderColor GetBorderColor(Xenon::Backend::BorderColor color) noexcept
	{
		switch (color)
		{
		case Xenon::Backend::BorderColor::TransparentBlackFLOAT:
			return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

		case Xenon::Backend::BorderColor::TransparentBlackINT:
			return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;

		case Xenon::Backend::BorderColor::OpaqueBlackFLOAT:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

		case Xenon::Backend::BorderColor::OpaqueBlackINT:
			return VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		case Xenon::Backend::BorderColor::OpaqueWhiteFLOAT:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

		case Xenon::Backend::BorderColor::OpaqueWhiteINT:
			return VK_BORDER_COLOR_INT_OPAQUE_WHITE;

		default:
			XENON_LOG_ERROR("Invalid border color!");
			return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		}
	}

	/**
	 * Get the compare operator.
	 *
	 * @param op The xenon operator.
	 * @return The Vulkan compare operator.
	 */
	[[nodiscard]] constexpr VkCompareOp GetCompareOperator(Xenon::Backend::CompareOperator op) noexcept
	{
		switch (op)
		{
		case Xenon::Backend::CompareOperator::Never:
			return VK_COMPARE_OP_NEVER;

		case Xenon::Backend::CompareOperator::Less:
			return VK_COMPARE_OP_LESS;

		case Xenon::Backend::CompareOperator::Equal:
			return VK_COMPARE_OP_EQUAL;

		case Xenon::Backend::CompareOperator::LessOrEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;

		case Xenon::Backend::CompareOperator::Greater:
			return VK_COMPARE_OP_GREATER;

		case Xenon::Backend::CompareOperator::NotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;

		case Xenon::Backend::CompareOperator::GreaterOrEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;

		case Xenon::Backend::CompareOperator::Always:
			return VK_COMPARE_OP_ALWAYS;

		default:
			XENON_LOG_ERROR("Invalid compare operator!");
			return VK_COMPARE_OP_NEVER;
		}
	}

	/**
	 * Get the filter type.
	 *
	 * @param filter The xenon filter.
	 * @return The VUlkan filter.
	 */
	[[nodiscard]] constexpr VkFilter GetFilter(Xenon::Backend::ImageFilter filter) noexcept
	{
		switch (filter)
		{
		case Xenon::Backend::ImageFilter::Nearest:
			return VK_FILTER_NEAREST;

		case Xenon::Backend::ImageFilter::Linear:
			return VK_FILTER_LINEAR;

		case Xenon::Backend::ImageFilter::CubicImage:
			return VK_FILTER_CUBIC_IMG;

		default:
			XENON_LOG_ERROR("Invalid image filter!");
			return VK_FILTER_NEAREST;
		}
	}

	/**
	 * Get the mip-map mode.
	 *
	 * @param mode The xenon mode.
	 * @return The Vulkan mode.
	 */
	[[nodiscard]] constexpr VkSamplerMipmapMode GetMipMapMode(Xenon::Backend::ImageMipMapMode mode) noexcept
	{
		switch (mode)
		{
		case Xenon::Backend::ImageMipMapMode::Nearest:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;

		case Xenon::Backend::ImageMipMapMode::Linear:
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;

		default:
			XENON_LOG_ERROR("Invalid image mip map filter!");
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanImageSampler::VulkanImageSampler(VulkanDevice* pDevice, const ImageSamplerSpecification& specification)
			: ImageSampler(pDevice, specification)
			, VulkanDeviceBoundObject(pDevice)
		{
			VkSamplerCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.addressModeU = GetAddressMode(specification.m_AddressModeU);
			createInfo.addressModeV = GetAddressMode(specification.m_AddressModeV);
			createInfo.addressModeW = GetAddressMode(specification.m_AddressModeW);
			createInfo.anisotropyEnable = XENON_VK_BOOL(specification.m_bEnableAnisotropy);
			createInfo.maxAnisotropy = specification.m_MaxAnisotrophy;
			createInfo.borderColor = GetBorderColor(specification.m_BorderColor);
			createInfo.compareEnable = XENON_VK_BOOL(specification.m_bEnableCompare);
			createInfo.compareOp = GetCompareOperator(specification.m_CompareOperator);
			createInfo.magFilter = GetFilter(specification.m_ImageMagificationFilter);
			createInfo.minFilter = GetFilter(specification.m_ImageMinificationFilter);
			createInfo.maxLod = specification.m_MaxLevelOfDetail;
			createInfo.minLod = specification.m_MinLevelOfDetail;
			createInfo.mipLodBias = specification.m_MipLevelOfDetailBias;
			createInfo.mipmapMode = GetMipMapMode(specification.m_MipMapMode);
			createInfo.unnormalizedCoordinates = XENON_VK_BOOL(specification.m_bEnableUnnormalizedCoordinates);

			// Get the supported max anisotropy from the physical device if needed.
			if (createInfo.maxAnisotropy == 0.0f && specification.m_bEnableAnisotropy)
			{
				VkPhysicalDeviceProperties vProperties = {};
				vkGetPhysicalDeviceProperties(pDevice->getPhysicalDevice(), &vProperties);
				createInfo.maxAnisotropy = vProperties.limits.maxSamplerAnisotropy;
			}

			XENON_VK_ASSERT(pDevice->getDeviceTable().vkCreateSampler(pDevice->getLogicalDevice(), &createInfo, nullptr, &m_Sampler), "Failed to create the image sampler!");
		}

		VulkanImageSampler::~VulkanImageSampler()
		{
			m_pDevice->getDeviceTable().vkDestroySampler(m_pDevice->getLogicalDevice(), m_Sampler, nullptr);
		}
	}
}