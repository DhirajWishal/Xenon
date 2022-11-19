// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderSource.hpp"
#include "../XenonCore/Logging.hpp"

#include <spirv_reflect.h>

#include <fstream>

namespace /* anonymous */
{
	/**
	 * Validate the reflection result.
	 *
	 * @param result The reflection result.
	 */
	void ValidateReflection(const SpvReflectResult result)
	{
		switch (result)
		{
		case SPV_REFLECT_RESULT_SUCCESS:										return;
		case SPV_REFLECT_RESULT_NOT_READY:										XENON_LOG_ERROR("Shader not ready!"); break;
		case SPV_REFLECT_RESULT_ERROR_PARSE_FAILED:								XENON_LOG_ERROR("Shader parse failed!"); break;
		case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED:								XENON_LOG_ERROR("Shader allocation failed!"); break;
		case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED:							XENON_LOG_ERROR("Shader range exceeded!"); break;
		case SPV_REFLECT_RESULT_ERROR_NULL_POINTER:								XENON_LOG_ERROR("Shader null pointer!"); break;
		case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR:							XENON_LOG_ERROR("Shader internal reflection error!"); break;
		case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH:							XENON_LOG_ERROR("Shader count mismatch!"); break;
		case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND:						XENON_LOG_ERROR("Shader element not found!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE:					XENON_LOG_ERROR("Shader invalid SPIRV code size!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER:				XENON_LOG_ERROR("Shader invalid SPIRV magic number!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF:						XENON_LOG_ERROR("Shader SPIRV unexpected end of file (EOF)!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE:				XENON_LOG_ERROR("Shader invalid SPIRV ID reference!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW:				XENON_LOG_ERROR("Shader invalid SPIRV descriptor set number overflow!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS:				XENON_LOG_ERROR("Shader invalid SPIRV storage class!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION:							XENON_LOG_ERROR("Shader invalid SPIRV recursion!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION:				XENON_LOG_ERROR("Shader invalid SPIRV instruction!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA:				XENON_LOG_ERROR("Shader invalid SPIRV block data!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE:		XENON_LOG_ERROR("Shader invalid SPIRV block member reference!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT:				XENON_LOG_ERROR("Shader invalid SPIRV entry point!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE:				XENON_LOG_ERROR("Shader invalid SPIRV execution mode!"); break;
		default:																XENON_LOG_ERROR("Unknown reflection error!");
		}
	}

	/**
	 * Get the binging type from the reflection type.
	 *
	 * @parma type The reflection type.
	 * @return The binding type.
	 */
	Xenon::Backend::ResourceType GetResourceType(SpvReflectDescriptorType type)
	{
		switch (type)
		{
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:								return Xenon::Backend::ResourceType::Sampler;
		case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:				return Xenon::Backend::ResourceType::CombinedImageSampler;
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:							return Xenon::Backend::ResourceType::SampledImage;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:							return Xenon::Backend::ResourceType::StorageImage;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:					return Xenon::Backend::ResourceType::UniformTexelBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:					return Xenon::Backend::ResourceType::StorageTexelBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:						return Xenon::Backend::ResourceType::UniformBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:						return Xenon::Backend::ResourceType::StorageBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:				return Xenon::Backend::ResourceType::DynamicUniformBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:				return Xenon::Backend::ResourceType::DynamicStorageBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:						return Xenon::Backend::ResourceType::InputAttachment;
		case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:			return Xenon::Backend::ResourceType::AccelerationStructure;
		default:																XENON_LOG_ERROR("Invalid resource type!"); return Xenon::Backend::ResourceType::UniformBuffer;
		}
	}

	/**
	 * Get the backend attribute vector data type from the dimensions.
	 *
	 * @param dimensions The vector's dimensions.
	 * @return The attribute data type.
	 */
	Xenon::Backend::AttributeDataType ResolveVectorDataType(uint32_t dimensions)
	{
		switch (dimensions)
		{
		case 2:
			return Xenon::Backend::AttributeDataType::Vec2;

		case 3:
			return Xenon::Backend::AttributeDataType::Vec3;

		case 4:
			return Xenon::Backend::AttributeDataType::Vec4;

		default:
			break;
		}

		return Xenon::Backend::AttributeDataType::Scalar;
	}
}

namespace Xenon
{
	namespace Backend
	{
		void ShaderSource::performReflection()
		{
			// Prepare the shader code for reflection.
			const auto reflectionSource = std::vector<uint32_t>(m_Binary.begin(), m_Binary.begin() + (m_Binary.size() / 4));

			SpvReflectShaderModule reflectionModule = {};
			ValidateReflection(spvReflectCreateShaderModule(reflectionSource.size() * sizeof(uint32_t), reflectionSource.data(), &reflectionModule));

			// Resolve shader inputs.
			{
				uint32_t variableCount = 0;
				ValidateReflection(spvReflectEnumerateInputVariables(&reflectionModule, &variableCount, nullptr));

				std::vector<SpvReflectInterfaceVariable*> pInputs(variableCount);
				ValidateReflection(spvReflectEnumerateInputVariables(&reflectionModule, &variableCount, pInputs.data()));

				// Iterate through the attributes and load them.
				for (auto& pResource : pInputs)
				{
					if (pResource->format == SPV_REFLECT_FORMAT_UNDEFINED || pResource->built_in != -1)
						continue;

					auto& input = m_InputAttributes.emplace_back();
					input.m_Location = pResource->location;
					input.m_DataType = ResolveVectorDataType(pResource->numeric.vector.component_count);
				}
			}

			// Resolve shader outputs.
			{
				uint32_t variableCount = 0;
				ValidateReflection(spvReflectEnumerateOutputVariables(&reflectionModule, &variableCount, nullptr));

				std::vector<SpvReflectInterfaceVariable*> pOutputs(variableCount);
				ValidateReflection(spvReflectEnumerateOutputVariables(&reflectionModule, &variableCount, pOutputs.data()));

				// Iterate through the attributes and load them.
				for (auto& pResource : pOutputs)
				{
					if (pResource->format == SPV_REFLECT_FORMAT_UNDEFINED || pResource->built_in != -1)
						continue;

					auto& output = m_OutputAttributes.emplace_back();
					output.m_Location = pResource->location;
					output.m_DataType = ResolveVectorDataType(pResource->numeric.vector.component_count);
				}
			}

			// Load all the bindings.
			{
				uint32_t variableCount = 0;
				ValidateReflection(spvReflectEnumerateDescriptorBindings(&reflectionModule, &variableCount, nullptr));

				std::vector<SpvReflectDescriptorBinding*> pBindings(variableCount);
				ValidateReflection(spvReflectEnumerateDescriptorBindings(&reflectionModule, &variableCount, pBindings.data()));

				// Iterate over the resources and setup the bindings.
				for (const auto& pResource : pBindings)
				{
					auto& resource = m_Resources.emplace_back();
					resource.m_Binding = pResource->binding;
					resource.m_Set = static_cast<DescriptorType>(pResource->set);
					resource.m_Type = GetResourceType(pResource->descriptor_type);
				}
			}
		}

		Xenon::Backend::ShaderSource ShaderSource::FromFile(const std::filesystem::path& shader)
		{
			std::vector<uint32_t> binaryData;
			auto shaderFile = std::fstream(shader, std::ios::in | std::ios::binary | std::ios::ate);

			// Load the data if we were able to open the file.
			if (shaderFile.is_open())
			{
				// Get the file size.
				const auto size = shaderFile.tellg();
				shaderFile.seekg(0);

				// Load the data to the vector.
				binaryData.resize(size);
				shaderFile.read(reinterpret_cast<char*>(binaryData.data()), size);

				// Close the file now.
				shaderFile.close();
			}
			else
			{
				XENON_LOG_ERROR("Failed to load the shader source @{}", shader.string());
			}

			return ShaderSource(std::move(binaryData));
		}
	}
}