// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "DX12ImageSampler.hpp"
#include "DX12Macros.hpp"

namespace /* anonymous */
{
	/**
	 * Get the D3D12 filter from the minification and magnification image filters from Xenon.
	 *
	 * @param minification The minification filter.
	 * @param magnification The magnification filter.
	 * @return The DX12 filter.
	 */
	[[nodiscard]] constexpr D3D12_FILTER GetFilter(Xenon::Backend::ImageFilter minification, Xenon::Backend::ImageFilter magnification) noexcept
	{
		D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

		if (minification == Xenon::Backend::ImageFilter::Nearest && magnification == Xenon::Backend::ImageFilter::Nearest)
			filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

		else if (minification == Xenon::Backend::ImageFilter::Nearest && magnification == Xenon::Backend::ImageFilter::Linear)
			filter = D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;

		else if (minification == Xenon::Backend::ImageFilter::Linear && magnification == Xenon::Backend::ImageFilter::Nearest)
			filter = D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;

		else if (minification == Xenon::Backend::ImageFilter::Linear && magnification == Xenon::Backend::ImageFilter::Linear)
			filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;

		else
			XENON_LOG_ERROR("Unsupported combination of minification and magnification image filters! Defaulting to Nearest (minification), Nearest (magnification).");

		return filter;
	}

	/**
	 * Get the D3D12 address mode from the Xenon address mode.
	 *
	 * @param addressMode The Xenon address mode.
	 * @return The DX12 address mode.
	 */
	[[nodiscard]] constexpr D3D12_TEXTURE_ADDRESS_MODE GetAddressMode(Xenon::Backend::AddressMode addressMode) noexcept
	{
		switch (addressMode)
		{
		case Xenon::Backend::AddressMode::Repeat:
			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;

		case Xenon::Backend::AddressMode::MirroredRepeat:
			return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;

		case Xenon::Backend::AddressMode::ClampToEdge:
			return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

		case Xenon::Backend::AddressMode::ClampToBorder:
			return D3D12_TEXTURE_ADDRESS_MODE_BORDER;

		case Xenon::Backend::AddressMode::MirrorClampToEdge:
			return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;

		default:
			XENON_LOG_ERROR("Invalid address mode! Defaulting to Repeat.");
			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}
	}

	/**
	 * Get the D3D12 comparison function from the Xenon compare operator.
	 *
	 * @param op The compare operator.
	 * @return The DX12 comparison function.
	 */
	[[nodiscard]] constexpr D3D12_COMPARISON_FUNC GetComparisonFunction(Xenon::Backend::CompareOperator op) noexcept
	{
		switch (op)
		{
		case Xenon::Backend::CompareOperator::Never:
			return D3D12_COMPARISON_FUNC_NEVER;

		case Xenon::Backend::CompareOperator::Less:
			return D3D12_COMPARISON_FUNC_LESS;

		case Xenon::Backend::CompareOperator::Equal:
			return D3D12_COMPARISON_FUNC_EQUAL;

		case Xenon::Backend::CompareOperator::LessOrEqual:
			return D3D12_COMPARISON_FUNC_LESS_EQUAL;

		case Xenon::Backend::CompareOperator::Greater:
			return D3D12_COMPARISON_FUNC_GREATER;

		case Xenon::Backend::CompareOperator::NotEqual:
			return D3D12_COMPARISON_FUNC_NOT_EQUAL;

		case Xenon::Backend::CompareOperator::GreaterOrEqual:
			return D3D12_COMPARISON_FUNC_GREATER_EQUAL;

		case Xenon::Backend::CompareOperator::Always:
			return D3D12_COMPARISON_FUNC_ALWAYS;

		default:
			XENON_LOG_ERROR("Invalid compare operator! Defaulting to Never.");
			return D3D12_COMPARISON_FUNC_NEVER;
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12ImageSampler::DX12ImageSampler(DX12Device* pDevice, const ImageSamplerSpecification& specification)
			: ImageSampler(pDevice, specification)
			, DX12DeviceBoundObject(pDevice)
		{
			m_Descriptor.Filter = GetFilter(specification.m_ImageMinificationFilter, specification.m_ImageMagificationFilter);
			m_Descriptor.AddressU = GetAddressMode(specification.m_AddressModeU);
			m_Descriptor.AddressV = GetAddressMode(specification.m_AddressModeV);
			m_Descriptor.AddressW = GetAddressMode(specification.m_AddressModeW);
			m_Descriptor.MipLODBias = specification.m_MipLevelOfDetailBias;
			// m_Descriptor.MaxAnisotropy = specification.m_MaxAnisotrophy == 0.0f ? UINT_MAX : static_cast<UINT>(specification.m_MaxAnisotrophy);
			m_Descriptor.MaxAnisotropy = specification.m_MaxAnisotrophy == 0.0f ? 1 : static_cast<UINT>(specification.m_MaxAnisotrophy);
			m_Descriptor.ComparisonFunc = GetComparisonFunction(specification.m_CompareOperator);
			m_Descriptor.MinLOD = specification.m_MinLevelOfDetail;
			m_Descriptor.MaxLOD = specification.m_MaxLevelOfDetail;

			switch (specification.m_BorderColor)
			{
			case Xenon::Backend::BorderColor::TransparentBlackFLOAT:
			case Xenon::Backend::BorderColor::TransparentBlackINT:
				m_Descriptor.BorderColor[0] = 0.0f;
				m_Descriptor.BorderColor[1] = 0.0f;
				m_Descriptor.BorderColor[2] = 0.0f;
				m_Descriptor.BorderColor[3] = 0.0f;
				break;

			case Xenon::Backend::BorderColor::OpaqueBlackFLOAT:
			case Xenon::Backend::BorderColor::OpaqueBlackINT:
				m_Descriptor.BorderColor[0] = 0.0f;
				m_Descriptor.BorderColor[1] = 0.0f;
				m_Descriptor.BorderColor[2] = 0.0f;
				m_Descriptor.BorderColor[3] = 1.0f;
				break;

			case Xenon::Backend::BorderColor::OpaqueWhiteFLOAT:
			case Xenon::Backend::BorderColor::OpaqueWhiteINT:
				m_Descriptor.BorderColor[0] = 1.0f;
				m_Descriptor.BorderColor[1] = 1.0f;
				m_Descriptor.BorderColor[2] = 1.0f;
				m_Descriptor.BorderColor[3] = 1.0f;
				break;

			default:
				break;
			}
		}
	}
}