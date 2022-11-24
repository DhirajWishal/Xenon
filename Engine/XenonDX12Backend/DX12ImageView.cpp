// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12ImageView.hpp"
#include "DX12Macros.hpp"

namespace /* anonymous */
{
	/**
	 * Get the component mapping for a single component.
	 *
	 * @param component The component swizzle.
	 * @param defaultValue The default value of the component.
	 * @return The component mapping.
	 */
	UINT GetComponentMapping(Xenon::Backend::ComponentSwizzle swizzle, UINT deafultValue)
	{
		switch (swizzle)
		{
		case Xenon::Backend::ComponentSwizzle::Identity:
			return deafultValue;

		case Xenon::Backend::ComponentSwizzle::Zero:
			return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0;

		case Xenon::Backend::ComponentSwizzle::One:
			return D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1;

		case Xenon::Backend::ComponentSwizzle::R:
			return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0;

		case Xenon::Backend::ComponentSwizzle::G:
			return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1;

		case Xenon::Backend::ComponentSwizzle::B:
			return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2;

		case Xenon::Backend::ComponentSwizzle::A:
			return D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3;

		default:
			return deafultValue;
		}
	}

	/**
	 * Get the shader 4 component mappings.
	 *
	 * @param r The red component swizzle.
	 * @param g The green component swizzle.
	 * @param b The blue component swizzle.
	 * @param a The alpha component swizzle.
	 * @return The component mapping.
	 */
	UINT GetShader4ComponentMapping(Xenon::Backend::ComponentSwizzle r, Xenon::Backend::ComponentSwizzle g, Xenon::Backend::ComponentSwizzle b, Xenon::Backend::ComponentSwizzle a)
	{
		const auto rComponent = GetComponentMapping(r, 0);
		const auto gComponent = GetComponentMapping(g, 0);
		const auto bComponent = GetComponentMapping(b, 0);
		const auto aComponent = GetComponentMapping(a, 0);

		return D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(rComponent, gComponent, bComponent, aComponent);
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12ImageView::DX12ImageView(DX12Device* pDevice, DX12Image* pImage, const ImageViewSpecification& specification)
			: ImageView(pDevice, pImage, specification)
			, DX12DeviceBoundObject(pDevice)
		{
			m_ShaderResouceView.Shader4ComponentMapping = GetShader4ComponentMapping(specification.m_ComponentR, specification.m_ComponentG, specification.m_ComponentB, specification.m_ComponentA);
			m_ShaderResouceView.Format = pDevice->convertFormat(pImage->getDataFormat());

			m_UnorderedAccessView.Format = pDevice->convertFormat(pImage->getDataFormat());

			switch (pImage->getSpecification().m_Type)
			{
			case Xenon::Backend::ImageType::OneDimensional:
				m_ShaderResouceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
				m_ShaderResouceView.Texture1D.MostDetailedMip = specification.m_LevelCount;
				m_ShaderResouceView.Texture1D.MipLevels = specification.m_BaseMipLevel;

				m_UnorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
				m_UnorderedAccessView.Texture2D.MipSlice = specification.m_BaseMipLevel;
				m_UnorderedAccessView.Texture2D.PlaneSlice = specification.m_BaseArrayLayer;
				break;

			case Xenon::Backend::ImageType::TwoDimensional:
				m_ShaderResouceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				m_ShaderResouceView.Texture2D.MostDetailedMip = specification.m_LevelCount;
				m_ShaderResouceView.Texture2D.MipLevels = specification.m_BaseMipLevel;
				m_ShaderResouceView.Texture2D.PlaneSlice = specification.m_BaseArrayLayer;

				m_UnorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				m_UnorderedAccessView.Texture2D.MipSlice = specification.m_BaseMipLevel;
				m_UnorderedAccessView.Texture2D.PlaneSlice = specification.m_BaseArrayLayer;
				break;

			case Xenon::Backend::ImageType::ThreeDImentional:
				m_ShaderResouceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
				m_ShaderResouceView.Texture3D.MostDetailedMip = specification.m_LevelCount;
				m_ShaderResouceView.Texture3D.MipLevels = specification.m_BaseMipLevel;

				m_UnorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
				m_UnorderedAccessView.Texture3D.MipSlice = specification.m_BaseMipLevel;
				m_UnorderedAccessView.Texture3D.WSize = specification.m_BaseArrayLayer;
				m_UnorderedAccessView.Texture3D.FirstWSlice = specification.m_BaseArrayLayer;
				break;

			case Xenon::Backend::ImageType::CubeMap:
				m_ShaderResouceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				m_ShaderResouceView.TextureCube.MostDetailedMip = specification.m_LevelCount;
				m_ShaderResouceView.TextureCube.MipLevels = specification.m_BaseMipLevel;

				m_UnorderedAccessView.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				m_UnorderedAccessView.Texture2D.MipSlice = specification.m_BaseMipLevel;
				m_UnorderedAccessView.Texture2D.PlaneSlice = specification.m_BaseArrayLayer;
				break;

			default:
				XENON_LOG_ERROR("Invalid or unsupported image type!");
				break;
			}
		}
	}
}