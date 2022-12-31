// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ShaderSource.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Shader attribute structure.
		 */
		struct ShaderAttribute final
		{
			uint32_t m_Location = 0;
			AttributeDataType m_DataType = AttributeDataType::Scalar;
		};

		/**
		 * Shader resource structure.
		 */
		struct ShaderResource final
		{
			uint32_t m_Binding = 0;

			DescriptorType m_Set = DescriptorType::Camera;
			ResourceType m_Type = ResourceType::Sampler;
		};

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
			explicit Shader(const ShaderSource& spirv);

			/**
			 * Explicit constructor.
			 *
			 * @param spirv The SPIR-V binary.
			 * @param dxil The DXIL binary.
			 */
			explicit Shader(const ShaderSource& spirv, const ShaderSource& dxil);

			/**
			 * Create a new shader.
			 *
			 * @tparam SPIRVType The SPIR-V binary type.
			 * @tparam SPIRVSize The SPIR-V binary size.
			 * @param pSPIRVSource The SPIR-V source array.
			 * @return The shader.
			 */
			template<class SPIRVType, uint64_t SPIRVSize>
			[[nodiscard]] static Shader Create(const SPIRVType(&pSPIRVSource)[SPIRVSize])
			{
				auto spirvSource = std::vector<uint32_t>(SPIRVSize / (sizeof(uint32_t) / sizeof(SPIRVType)));
				std::copy_n(pSPIRVSource, SPIRVSize, reinterpret_cast<std::remove_const_t<SPIRVType>*>(spirvSource.data()));

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
			[[nodiscard]] static Shader Create(const SPIRVType(&pSPIRVSource)[SPIRVSize], const DXILType(&pDXILSource)[DXILSize])
			{
				auto spirvSource = std::vector<uint32_t>(SPIRVSize / (sizeof(uint32_t) / sizeof(SPIRVType)));
				std::copy_n(pSPIRVSource, SPIRVSize, reinterpret_cast<std::remove_const_t<SPIRVType>*>(spirvSource.data()));

				auto dxilSource = std::vector<uint32_t>(DXILSize / (sizeof(uint32_t) / sizeof(DXILType)));
				std::copy_n(pDXILSource, DXILSize, reinterpret_cast<std::remove_const_t<DXILType>*>(dxilSource.data()));

				return Shader(ShaderSource(std::move(spirvSource)), ShaderSource(std::move(dxilSource)));
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

			/**
			 * Get the shader's input attributes.
			 *
			 * @return The input attributes.
			 */
			[[nodiscard]] const std::vector<ShaderAttribute>& getInputAttributes() const noexcept { return m_InputAttributes; }

			/**
			 * Get the shader's output attributes.
			 *
			 * @return The output attributes.
			 */
			[[nodiscard]] const std::vector<ShaderAttribute>& getOutputAttributes() const noexcept { return m_OutputAttributes; }

			/**
			 * Get the resources.
			 *
			 * @return The shader resources.
			 */
			[[nodiscard]] const std::vector<ShaderResource>& getResources() const noexcept { return m_Resources; }

		private:
			/**
			 * Perform reflection over the binary source and get information about inputs, outputs and resources.
			 */
			void performReflection();

		private:
			ShaderSource m_SPIRV;
			ShaderSource m_DXIL;

			std::vector<ShaderAttribute> m_InputAttributes;
			std::vector<ShaderAttribute> m_OutputAttributes;
			std::vector<ShaderResource> m_Resources;
		};
	}
}