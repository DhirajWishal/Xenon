// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderSource.hpp"

#include <unordered_map>
#include <string>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Shader class.
		 * This class is used to store information about a single shader file.
		 *
		 * In Xenon, a shader file outputs 2 shader binaries (on Windows).
		 * 1. SPIR-V.
		 * 2. DXIL.
		 * 
		 * The actual backend will select which shader it may need. Because of this, the shader class holds two shader sources (to make the API simpler).
		 * On Windows, both needs to be set. On Linux and any other platform, only the SPIR-V shader can be set (since only the Vulkan backend is supported).
		 */
		class Shader final
		{
		public:
			/**
			 * Default constructor.
			 */
			Shader() = default;

			/**
			 * Explicit constructor.
			 *
			 * @param spirv The SPIR-V binary.
			 */
			explicit Shader(const ShaderSource& spirv) : m_SPIRV(spirv) {}

			/**
			 * Explicit constructor.
			 *
			 * @param spirv The SPIR-V binary.
			 * @param dxil The DXIL binary.
			 */
			explicit Shader(const ShaderSource& spirv, const ShaderSource& dxil) : m_SPIRV(spirv), m_DXIL(dxil) {}

			/**
			 * Create a new shader.
			 *
			 * @tparam SPIRVType The SPIR-V binary type.
			 * @tparam SPIRVSize The SPIR-V binary size.
			 * @param pSPIRVSource The SPIR-V source array.
			 * @return The shader.
			 */
			template<class SPIRVType, uint64_t SPIRVSize>
			[[nodiscard]] static Shader Create(SPIRVType(&pSPIRVSource)[SPIRVSize])
			{
				auto spirvSource = std::vector<uint32_t>(SPIRVSize);
				std::copy_n(pSPIRVSource, spirvSource.size(), spirvSource.begin());

				return Shader(ShaderSource(std::move(spirvSource)));
			}

			/**
			 * Create a new shader.
			 *
			 * @tparam SPIRVType The SPIR-V binary type.
			 * @tparam SPIRVSize The SPIR-V binary size.
			 * @tparam DXILType The DXIL binary type.
			 * @tparam DXILSize The DXIL binary size.
			 * @param pSPIRVSource The SPIR-V source array.
			 * @param pDXILSource The HLSL source array.
			 * @return The shader.
			 */
			template<class SPIRVType, uint64_t SPIRVSize, class DXILType, uint64_t DXILSize>
			[[nodiscard]] static Shader Create(SPIRVType(&pSPIRVSource)[SPIRVSize], DXILType(&pDXILSource)[DXILSize])
			{
				auto spirvSource = std::vector<uint32_t>(SPIRVSize);
				std::copy_n(pSPIRVSource, spirvSource.size(), spirvSource.begin());

				auto dxilSource = std::vector<uint32_t>(DXILSize / sizeof(uint32_t));
				std::copy_n(pDXILSource, dxilSource.size(), reinterpret_cast<unsigned char*>(dxilSource.data()));

				return Shader(ShaderSource(std::move(spirvSource)), ShaderSource(std::move(dxilSource), "main", false));
			}

			/**
			 * Get the SPIR-V shader source.
			 *
			 * @return The shader source.
			 */
			[[nodiscard]] const ShaderSource& getSPIRV() const noexcept { return m_SPIRV; }

			/**
			 * Get the DXIL shader source.
			 *
			 * @return The shader source.
			 */
			[[nodiscard]] const ShaderSource& getDXIL() const noexcept { return m_DXIL; }

		private:
			ShaderSource m_SPIRV;
			ShaderSource m_DXIL;
		};
	}
}