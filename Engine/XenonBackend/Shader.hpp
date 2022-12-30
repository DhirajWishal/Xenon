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
		 * 2. HLSL binary.
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
			 * @param hlsl The HLSL binary.
			 */
			explicit Shader(const ShaderSource& spirv, const ShaderSource& hlsl) : m_SPIRV(spirv), m_HLSL(hlsl) {}

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
			 * @tparam HLSLType The HLSL binary type.
			 * @tparam HLSLSize The HLSL binary size.
			 * @param pSPIRVSource The SPIR-V source array.
			 * @param pHLSLSource The HLSL source array.
			 * @return The shader.
			 */
			template<class SPIRVType, uint64_t SPIRVSize, class HLSLType, uint64_t HLSLSize>
			[[nodiscard]] static Shader Create(SPIRVType(&pSPIRVSource)[SPIRVSize], HLSLType(&pHLSLSource)[HLSLSize])
			{
				auto spirvSource = std::vector<uint32_t>(SPIRVSize);
				std::copy_n(pSPIRVSource, spirvSource.size(), spirvSource.begin());

				auto hlslSource = std::vector<uint32_t>(HLSLSize / sizeof(uint32_t));
				std::copy_n(pHLSLSource, hlslSource.size(), reinterpret_cast<unsigned char*>(hlslSource.data()));

				return Shader(ShaderSource(std::move(spirvSource)), ShaderSource(std::move(hlslSource), "main", false));
			}

			/**
			 * Get the SPIR-V shader source.
			 *
			 * @return The shader source.
			 */
			[[nodiscard]] const ShaderSource& getSPIRV() const noexcept { return m_SPIRV; }

			/**
			 * Get the HLSL shader source.
			 *
			 * @return The shader source.
			 */
			[[nodiscard]] const ShaderSource& getHLSL() const noexcept { return m_HLSL; }

		private:
			ShaderSource m_SPIRV;
			ShaderSource m_HLSL;
		};
	}
}