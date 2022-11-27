// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12RasterizingPipeline.hpp"
#include "DX12Macros.hpp"

#include <spirv_hlsl.hpp>

namespace /* anonymous */
{
	/**
	 * Get the descriptor range type.
	 *
	 * @param resource The Xenon resource type.
	 * @return The D3D12 descriptor range type.
	 */
	[[nodiscard]] constexpr D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangeType(Xenon::Backend::ResourceType resource) noexcept
	{
		switch (resource)
		{
		case Xenon::Backend::ResourceType::Sampler:
		case Xenon::Backend::ResourceType::CombinedImageSampler:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;

		case Xenon::Backend::ResourceType::SampledImage:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::StorageImage:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::UniformTexelBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::StorageTexelBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::UniformBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::StorageBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::DynamicUniformBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::DynamicStorageBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::InputAttachment:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::AccelerationStructure:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		default:
			XENON_LOG_ERROR("Invalid resource type! Defaulting to SRV.");
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		}
	}

	/**
	 * Setup all the shader-specific data.
	 *
	 * @param shader The shader to get the data from.
	 * @param rangeMap The descriptor range map.
	 */
	void SetupShaderData(
		const Xenon::Backend::ShaderSource& shader,
		std::unordered_map<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>& rangeMap,
		std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs,
		Xenon::Backend::ShaderType type)
	{
		// Setup resources.
		for (const auto& resource : shader.getResources())
		{
			const auto rangeType = GetDescriptorRangeType(resource.m_Type);
			const auto index = (Xenon::EnumToInt(resource.m_Set) * 2) + (rangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER ? 1 : 0);
			rangeMap[static_cast<uint8_t>(index)].emplace_back().Init(rangeType, 1, resource.m_Binding);
		}

		// Setup the inputs if it's the vertex shader.
		if (type & Xenon::Backend::ShaderType::Vertex)
		{
			for (const auto& input : shader.getInputAttributes())
			{
				const auto element = static_cast<Xenon::Backend::InputElement>(input.m_Location);

				auto& desc = inputs.emplace_back();
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.InputSlot = 0;
				desc.AlignedByteOffset = 0;
				desc.InputSlotClass = Xenon::Backend::IsVertexElement(element) ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
				desc.InstanceDataStepRate = 0;

				switch (element)
				{
				case Xenon::Backend::InputElement::VertexPosition:
					desc.SemanticName = "POSITION";
					desc.SemanticIndex = 0;
					break;

				case Xenon::Backend::InputElement::VertexNormal:
					desc.SemanticName = "NORMAL";
					desc.SemanticIndex = 0;
					break;

				case Xenon::Backend::InputElement::VertexTangent:
					desc.SemanticName = "TANGENT";
					desc.SemanticIndex = 0;
					break;

				case Xenon::Backend::InputElement::VertexColor_0:
					desc.SemanticName = "COLOR";
					desc.SemanticIndex = 0;
					break;

				case Xenon::Backend::InputElement::VertexColor_1:
					desc.SemanticName = "COLOR";
					desc.SemanticIndex = 1;
					break;

				case Xenon::Backend::InputElement::VertexColor_2:
					desc.SemanticName = "COLOR";
					desc.SemanticIndex = 2;
					break;

				case Xenon::Backend::InputElement::VertexColor_3:
					desc.SemanticName = "COLOR";
					desc.SemanticIndex = 3;
					break;

				case Xenon::Backend::InputElement::VertexColor_4:
					desc.SemanticName = "COLOR";
					desc.SemanticIndex = 4;
					break;

				case Xenon::Backend::InputElement::VertexColor_5:
					desc.SemanticName = "COLOR";
					desc.SemanticIndex = 5;
					break;

				case Xenon::Backend::InputElement::VertexColor_6:
					desc.SemanticName = "COLOR";
					desc.SemanticIndex = 6;
					break;

				case Xenon::Backend::InputElement::VertexColor_7:
					desc.SemanticName = "COLOR";
					desc.SemanticIndex = 7;
					break;

				case Xenon::Backend::InputElement::VertexTextureCoordinate_0:
					desc.SemanticName = "TEXCOORD";
					desc.SemanticIndex = 0;
					break;

				case Xenon::Backend::InputElement::VertexTextureCoordinate_1:
					desc.SemanticName = "TEXCOORD";
					desc.SemanticIndex = 1;
					break;

				case Xenon::Backend::InputElement::VertexTextureCoordinate_2:
					desc.SemanticName = "TEXCOORD";
					desc.SemanticIndex = 2;
					break;

				case Xenon::Backend::InputElement::VertexTextureCoordinate_3:
					desc.SemanticName = "TEXCOORD";
					desc.SemanticIndex = 3;
					break;

				case Xenon::Backend::InputElement::VertexTextureCoordinate_4:
					desc.SemanticName = "TEXCOORD";
					desc.SemanticIndex = 4;
					break;

				case Xenon::Backend::InputElement::VertexTextureCoordinate_5:
					desc.SemanticName = "TEXCOORD";
					desc.SemanticIndex = 5;
					break;

				case Xenon::Backend::InputElement::VertexTextureCoordinate_6:
					desc.SemanticName = "TEXCOORD";
					desc.SemanticIndex = 6;
					break;

				case Xenon::Backend::InputElement::VertexTextureCoordinate_7:
					desc.SemanticName = "TEXCOORD";
					desc.SemanticIndex = 7;
					break;

				case Xenon::Backend::InputElement::InstancePosition:
					desc.SemanticName = "POSITION";
					desc.SemanticIndex = 1;
					desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					desc.InputSlot = 1;
					desc.AlignedByteOffset = offsetof(Xenon::Backend::InstanceEntry, m_Position);
					break;

				case Xenon::Backend::InputElement::InstanceRotation:
					desc.SemanticName = "POSITION";
					desc.SemanticIndex = 2;
					desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					desc.InputSlot = 1;
					desc.AlignedByteOffset = offsetof(Xenon::Backend::InstanceEntry, m_Rotation);
					break;

				case Xenon::Backend::InputElement::InstanceScale:
					desc.SemanticName = "POSITION";
					desc.SemanticIndex = 3;
					desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					desc.InputSlot = 1;
					desc.AlignedByteOffset = offsetof(Xenon::Backend::InstanceEntry, m_Scale);
					break;

				case Xenon::Backend::InputElement::InstanceID:
					desc.SemanticName = "PSIZE";
					desc.SemanticIndex = 1;
					desc.Format = DXGI_FORMAT_R32_FLOAT;
					desc.InputSlot = 1;
					desc.AlignedByteOffset = offsetof(Xenon::Backend::InstanceEntry, m_InstanceID);
					break;

				default:
					XENON_LOG_ERROR("Invalid or unsupported input type!");
					break;
				}
			}
		}
	}

	/**
	 * Compile a shader to a D3D12 shader blob.
	 *
	 * @param shader The shader to compile.
	 * @return The shader binary.
	 */
	ComPtr<ID3DBlob> CompileShader(const Xenon::Backend::ShaderSource& shader, const std::string_view& target)
	{
		ComPtr<ID3DBlob> shaderBlob;

		try
		{
			// Remove the end padding and create the compiler.
			auto compiler = spirv_cross::CompilerHLSL(shader.getBinaryWithoutPadding());

			// Cross-compile the binary.
			const auto hlsl = compiler.compile();
			XENON_LOG_INFORMATION("Compiled shader code.\n{}", hlsl);

			// Set the options.
			spirv_cross::CompilerHLSL::Options options;
			options.shader_model = 50;

			compiler.set_hlsl_options(options);

#ifdef XENON_DEBUG
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

#else
			UINT compileFlags = 0;

#endif

			ComPtr<ID3DBlob> error;
			XENON_DX12_ASSERT(D3DCompile(hlsl.data(), hlsl.size(), nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target.data(), compileFlags, 0, &shaderBlob, &error), "Failed to compile the shader!");
			XENON_DX12_ASSERT_BLOB(error);
		}
		catch (const std::exception& e)
		{
			XENON_LOG_FATAL("An exception was thrown when cross-compiling SPI-V to HLSL! {}", e.what());
		}

		return shaderBlob;
	}

	/**
	 * Get the fill mode.
	 *
	 * @param mode The polygon mode.
	 * @return The D3D12 fill mode.
	 */
	[[nodiscard]] constexpr D3D12_FILL_MODE GetFillMode(Xenon::Backend::PolygonMode mode) noexcept
	{
		switch (mode)
		{
		case Xenon::Backend::PolygonMode::Fill:
			return D3D12_FILL_MODE_SOLID;

		case Xenon::Backend::PolygonMode::Line:
			return D3D12_FILL_MODE_WIREFRAME;

		default:
			XENON_LOG_ERROR("Invalid or unsupported polygon mode! Defaulting to Fill");
			return D3D12_FILL_MODE_SOLID;
		}
	}

	/**
	 * Get the cull mode.
	 *
	 * @param cull The flint cull mode.
	 * @return The D3D12 cull mode.
	 */
	[[nodiscard]] constexpr D3D12_CULL_MODE GetCullMode(Xenon::Backend::CullMode cull) noexcept
	{
		switch (cull)
		{
		case Xenon::Backend::CullMode::None:
			return D3D12_CULL_MODE_NONE;

		case Xenon::Backend::CullMode::Front:
			return D3D12_CULL_MODE_FRONT;

		case Xenon::Backend::CullMode::Back:
			return D3D12_CULL_MODE_BACK;

		default:
			XENON_LOG_ERROR("Invalid or unsupported cull mode! Defaulting to None.");
			return D3D12_CULL_MODE_NONE;
		}
	}

	/**
	 * Get the front face.
	 *
	 * @param face The flint front face.
	 * @return The D3D12 front face.
	 */
	[[nodiscard]] constexpr BOOL GetFrontFace(Xenon::Backend::FrontFace face) noexcept
	{
		switch (face)
		{
		case Xenon::Backend::FrontFace::CounterClockwise:
			return TRUE;

		case Xenon::Backend::FrontFace::Clockwise:
			return FALSE;

		default:
			XENON_LOG_ERROR("Invalid or Undefined front face! Defaulting to CounterClockwise.");
			return TRUE;
		}
	}

	/**
	 * Get the blend.
	 *
	 * @param blendFactor The blend factor.
	 * @return The D3D12 blend.
	 */
	[[nodiscard]] constexpr D3D12_BLEND GetBlend(Xenon::Backend::ColorBlendFactor blendFactor) noexcept
	{
		switch (blendFactor)
		{
		case Xenon::Backend::ColorBlendFactor::Zero:
			return D3D12_BLEND_ZERO;

		case Xenon::Backend::ColorBlendFactor::One:
			return D3D12_BLEND_ONE;

		case Xenon::Backend::ColorBlendFactor::SourceColor:
			return D3D12_BLEND_SRC_COLOR;

		case Xenon::Backend::ColorBlendFactor::OneMinusSourceColor:
			return D3D12_BLEND_INV_SRC_COLOR;

		case Xenon::Backend::ColorBlendFactor::DestinationColor:
			return D3D12_BLEND_DEST_COLOR;

		case Xenon::Backend::ColorBlendFactor::OneMinusDestinationColor:
			return D3D12_BLEND_INV_DEST_COLOR;

		case Xenon::Backend::ColorBlendFactor::SourceAlpha:
			return D3D12_BLEND_SRC_ALPHA;

		case Xenon::Backend::ColorBlendFactor::OneMinusSourceAlpha:
			return D3D12_BLEND_INV_SRC_ALPHA;

		case Xenon::Backend::ColorBlendFactor::DestinationAlpha:
			return D3D12_BLEND_DEST_ALPHA;

		case Xenon::Backend::ColorBlendFactor::OneMinusDestinationAlpha:
			return D3D12_BLEND_INV_DEST_ALPHA;

		default:
			XENON_LOG_ERROR("Invalid or unsupported color blend factor! Defaulting to Zero.");
			return D3D12_BLEND_ZERO;
		}
	}

	/**
	 * Get the color blend operator.
	 *
	 * @param blendOperator The blend operator.
	 * @return The D3D12 blend operator.
	 */
	[[nodiscard]] constexpr D3D12_BLEND_OP GetBlendOperator(Xenon::Backend::ColorBlendOperator blendOperator) noexcept
	{
		switch (blendOperator)
		{
		case Xenon::Backend::ColorBlendOperator::Add:
			return D3D12_BLEND_OP_ADD;

		case Xenon::Backend::ColorBlendOperator::Subtract:
			return D3D12_BLEND_OP_SUBTRACT;

		case Xenon::Backend::ColorBlendOperator::ReverseSubtract:
			return D3D12_BLEND_OP_REV_SUBTRACT;

		case Xenon::Backend::ColorBlendOperator::Minimum:
			return D3D12_BLEND_OP_MIN;

		case Xenon::Backend::ColorBlendOperator::Maximum:
			return D3D12_BLEND_OP_MAX;

		default:
			XENON_LOG_ERROR("Invalid or unsupported color blend operator! Defaulting to Add.");
			return D3D12_BLEND_OP_ADD;
		}
	}

	/**
	 * Get the color blend logic operator.
	 *
	 * @param logic The Xenon logic.
	 * @return The D3D12 logic operator.
	 */
	[[nodiscard]] constexpr D3D12_LOGIC_OP GetColorBlendLogic(Xenon::Backend::ColorBlendLogic logic) noexcept
	{
		switch (logic)
		{
		case Xenon::Backend::ColorBlendLogic::Clear:
			return D3D12_LOGIC_OP_CLEAR;

		case Xenon::Backend::ColorBlendLogic::And:
			return D3D12_LOGIC_OP_AND;

		case Xenon::Backend::ColorBlendLogic::AndReverse:
			return D3D12_LOGIC_OP_AND_REVERSE;

		case Xenon::Backend::ColorBlendLogic::Copy:
			return D3D12_LOGIC_OP_COPY;

		case Xenon::Backend::ColorBlendLogic::AndInverted:
			return D3D12_LOGIC_OP_AND_INVERTED;

		case Xenon::Backend::ColorBlendLogic::NoOperator:
			return D3D12_LOGIC_OP_NOOP;

		case Xenon::Backend::ColorBlendLogic::XOR:
			return D3D12_LOGIC_OP_XOR;

		case Xenon::Backend::ColorBlendLogic::OR:
			return D3D12_LOGIC_OP_OR;

		case Xenon::Backend::ColorBlendLogic::NOR:
			return D3D12_LOGIC_OP_NOR;

		case Xenon::Backend::ColorBlendLogic::Equivalent:
			return D3D12_LOGIC_OP_EQUIV;

		case Xenon::Backend::ColorBlendLogic::Invert:
			return D3D12_LOGIC_OP_INVERT;

		case Xenon::Backend::ColorBlendLogic::ReverseOR:
			return D3D12_LOGIC_OP_OR_REVERSE;

		case Xenon::Backend::ColorBlendLogic::CopyInverted:
			return D3D12_LOGIC_OP_COPY_INVERTED;

		case Xenon::Backend::ColorBlendLogic::InvertedOR:
			return D3D12_LOGIC_OP_COPY_INVERTED;

		case Xenon::Backend::ColorBlendLogic::NAND:
			return D3D12_LOGIC_OP_NAND;

		case Xenon::Backend::ColorBlendLogic::Set:
			return D3D12_LOGIC_OP_SET;

		default:
			XENON_LOG_ERROR("Invalid or unsupported color blend operator! Defaulting to Clear.");
			return D3D12_LOGIC_OP_CLEAR;
		}
	}

	/**
	 * Get the color write enable.
	 *
	 * @param writeMash The Xenon write mask.
	 * @return The D3D12 write enable.
	 */
	[[nodiscard]] constexpr D3D12_COLOR_WRITE_ENABLE GetWriteEnable(Xenon::Backend::ColorWriteMask writeMask)
	{
		if (writeMask & Xenon::Backend::ColorWriteMask::R &&
			writeMask & Xenon::Backend::ColorWriteMask::G &&
			writeMask & Xenon::Backend::ColorWriteMask::B &&
			writeMask & Xenon::Backend::ColorWriteMask::A)
			return D3D12_COLOR_WRITE_ENABLE_ALL;

		if (writeMask & Xenon::Backend::ColorWriteMask::R)
			return D3D12_COLOR_WRITE_ENABLE_RED;

		if (writeMask & Xenon::Backend::ColorWriteMask::G)
			return D3D12_COLOR_WRITE_ENABLE_GREEN;

		if (writeMask & Xenon::Backend::ColorWriteMask::B)
			return D3D12_COLOR_WRITE_ENABLE_BLUE;

		if (writeMask & Xenon::Backend::ColorWriteMask::A)
			return D3D12_COLOR_WRITE_ENABLE_ALPHA;

		XENON_LOG_ERROR("Invalid color write mask! Defaulting to all R, G, B and A.");
		return D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	/**
	 * Get the depth comparison logic.
	 *
	 * @param logic The logic.
	 * @return The D3D12 comparison function.
	 */
	[[nodiscard]] constexpr D3D12_COMPARISON_FUNC GetComparisonFunction(Xenon::Backend::DepthCompareLogic logic)
	{
		switch (logic)
		{
		case Xenon::Backend::DepthCompareLogic::Never:
			return D3D12_COMPARISON_FUNC_NEVER;

		case Xenon::Backend::DepthCompareLogic::Less:
			return D3D12_COMPARISON_FUNC_LESS;

		case Xenon::Backend::DepthCompareLogic::Equal:
			return D3D12_COMPARISON_FUNC_EQUAL;

		case Xenon::Backend::DepthCompareLogic::LessOrEqual:
			return D3D12_COMPARISON_FUNC_LESS_EQUAL;

		case Xenon::Backend::DepthCompareLogic::Greater:
			return D3D12_COMPARISON_FUNC_GREATER;

		case Xenon::Backend::DepthCompareLogic::NotEqual:
			return D3D12_COMPARISON_FUNC_NOT_EQUAL;

		case Xenon::Backend::DepthCompareLogic::GreaterOrEqual:
			return D3D12_COMPARISON_FUNC_GREATER_EQUAL;

		case Xenon::Backend::DepthCompareLogic::Always:
			return D3D12_COMPARISON_FUNC_ALWAYS;

		default:
			XENON_LOG_ERROR("Invalid or unsupported depth compare logic! Defaulting to Never.");
			return D3D12_COMPARISON_FUNC_NEVER;
		}
	}

	/**
	 * Get the element format from the component count and the size.
	 *
	 * @param componentCount The number of components.
	 * @param dataType The component data type.
	 * @return The DXGI format.
	 */
	[[nodiscard]] constexpr DXGI_FORMAT GetElementFormat(uint8_t componentCount, Xenon::Backend::ComponentDataType dataType) noexcept
	{
		if (componentCount == 1)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return DXGI_FORMAT_R8_UINT;

			case Xenon::Backend::ComponentDataType::Uint16:
				return DXGI_FORMAT_R16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return DXGI_FORMAT_R32_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return DXGI_FORMAT_R8_SINT;

			case Xenon::Backend::ComponentDataType::Int16:
				return DXGI_FORMAT_R16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return DXGI_FORMAT_R32_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return DXGI_FORMAT_R32_FLOAT;

			default:
				break;
			}
		}
		else if (componentCount == 2)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return DXGI_FORMAT_R8G8_UINT;

			case Xenon::Backend::ComponentDataType::Uint16:
				return DXGI_FORMAT_R16G16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return DXGI_FORMAT_R32G32_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return DXGI_FORMAT_R8G8_SINT;

			case Xenon::Backend::ComponentDataType::Int16:
				return DXGI_FORMAT_R16G16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return DXGI_FORMAT_R32G32_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return DXGI_FORMAT_R32G32_FLOAT;

			default:
				break;
			}
		}
		else if (componentCount == 3)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint32:
				return DXGI_FORMAT_R32G32B32_UINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return DXGI_FORMAT_R32G32B32_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return DXGI_FORMAT_R32G32B32_FLOAT;

			default:
				break;
			}
		}
		else if (componentCount == 4)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return DXGI_FORMAT_R8G8B8A8_UINT;

			case Xenon::Backend::ComponentDataType::Uint16:
				return DXGI_FORMAT_R16G16B16A16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return DXGI_FORMAT_R32G32B32A32_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return DXGI_FORMAT_R8G8B8A8_SINT;

			case Xenon::Backend::ComponentDataType::Int16:
				return DXGI_FORMAT_R16G16B16A16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return DXGI_FORMAT_R32G32B32A32_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;

			default:
				break;
			}
		}

		XENON_LOG_ERROR("There are no available types for the given component count ({}) and component data type ({})!", componentCount, Xenon::EnumToInt(dataType));
		return DXGI_FORMAT_UNKNOWN;
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12RasterizingPipeline::DX12RasterizingPipeline(DX12Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, DX12Rasterizer* pRasterizer, const RasterizingPipelineSpecification& specification)
			: RasterizingPipeline(pDevice, std::move(pCacheHandler), pRasterizer, specification)
			, DX12DeviceBoundObject(pDevice)
			, m_pRasterizer(pRasterizer)
		{
			// Resolve shader-specific data.
			std::unordered_map<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>> rangeMap;
			if (specification.m_VertexShader.isValid())
			{
				SetupShaderData(specification.m_VertexShader, rangeMap, m_Inputs, ShaderType::Vertex);
				m_VertexShader = CompileShader(specification.m_VertexShader, "vs_5_0");
			}

			if (specification.m_FragmentShader.isValid())
			{
				SetupShaderData(specification.m_FragmentShader, rangeMap, m_Inputs, ShaderType::Fragment);
				m_PixelShader = CompileShader(specification.m_FragmentShader, "ps_5_0");
			}

			// Create the root signature.
			createRootSignature(std::move(rangeMap));

			// Setup the pipeline descriptor.
			setupPipelineStateDescriptor();
		}

		const Xenon::Backend::DX12RasterizingPipeline::PipelineStorage& DX12RasterizingPipeline::getPipeline(const VertexSpecification& vertexSpecification)
		{
			const auto hash = GenerateHashFor(vertexSpecification);

			if (!m_Pipelines.contains(hash))
			{
				auto& pipeline = m_Pipelines[hash];
				pipeline.m_Inputs = m_Inputs;

				auto itr = m_Specification.m_VertexShader.getInputAttributes().begin();
				for (auto& input : pipeline.m_Inputs)
				{
					// Continue if the input is instance data.
					if (input.InputSlotClass == D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA)
					{
						++itr;
						continue;
					}

					const auto element = static_cast<InputElement>(itr->m_Location);

					// If we have the input element in the vertex specification, we can setup the information.
					if (vertexSpecification.isAvailable(static_cast<InputElement>(itr->m_Location)))
					{
						input.AlignedByteOffset = vertexSpecification.offsetOf(element);
						input.Format = GetElementFormat(
							GetAttributeDataTypeComponentCount(GetInputElementDataType(element)),
							vertexSpecification.getElementComponentDataType(element)
						);
					}

					++itr;
				}

				// Create the pipeline.
				D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineState = m_PipelineStateDescriptor;
				pipelineState.InputLayout.pInputElementDescs = pipeline.m_Inputs.data();
				pipelineState.InputLayout.NumElements = static_cast<UINT>(pipeline.m_Inputs.size());

				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateGraphicsPipelineState(&pipelineState, IID_PPV_ARGS(&pipeline.m_PipelineState)), "Failed to create the pipeline state!");
			}

			return m_Pipelines[hash];
		}

		void DX12RasterizingPipeline::createRootSignature(std::unordered_map<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>&& rangeMap)
		{
			std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
			for (const auto& [set, ranges] : rangeMap)
				rootParameters.emplace_back().InitAsDescriptorTable(static_cast<UINT>(ranges.size()), ranges.data(), D3D12_SHADER_VISIBILITY_ALL);

			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (FAILED(m_pDevice->getDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

			// Create the root signature.
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(static_cast<UINT>(rootParameters.size()), rootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			XENON_DX12_ASSERT(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error), "Failed to serialize the version-ed root signature!");
			XENON_DX12_ASSERT_BLOB(error);

			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)), "Failed to create the root signature!");
		}

		void DX12RasterizingPipeline::setupPipelineStateDescriptor()
		{
			// m_PipelineStateDescriptor.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			m_PipelineStateDescriptor.pRootSignature = m_RootSignature.Get();
			m_PipelineStateDescriptor.VS = CD3DX12_SHADER_BYTECODE(m_VertexShader.Get());
			m_PipelineStateDescriptor.PS = CD3DX12_SHADER_BYTECODE(m_PixelShader.Get());

			m_PipelineStateDescriptor.RasterizerState.FillMode = GetFillMode(m_Specification.m_PolygonMode);
			m_PipelineStateDescriptor.RasterizerState.CullMode = GetCullMode(m_Specification.m_CullMode);
			m_PipelineStateDescriptor.RasterizerState.FrontCounterClockwise = GetFrontFace(m_Specification.m_FrontFace);
			m_PipelineStateDescriptor.RasterizerState.DepthBias = static_cast<INT>(m_Specification.m_DepthBiasFactor);
			m_PipelineStateDescriptor.RasterizerState.SlopeScaledDepthBias = m_Specification.m_DepthSlopeFactor;
			m_PipelineStateDescriptor.RasterizerState.DepthClipEnable = m_Specification.m_EnableDepthClamp;
			m_PipelineStateDescriptor.RasterizerState.MultisampleEnable = m_Specification.m_EnableSampleShading;
			m_PipelineStateDescriptor.RasterizerState.AntialiasedLineEnable = TRUE;
			m_PipelineStateDescriptor.RasterizerState.ForcedSampleCount = 0;
			m_PipelineStateDescriptor.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

			m_PipelineStateDescriptor.BlendState.AlphaToCoverageEnable = FALSE;
			m_PipelineStateDescriptor.BlendState.IndependentBlendEnable = FALSE;

			for (uint8_t i = 0; i < m_Specification.m_ColorBlendAttachments.size(); i++)
			{
				const auto& attachment = m_Specification.m_ColorBlendAttachments[i];

				m_PipelineStateDescriptor.BlendState.RenderTarget[i].BlendEnable = attachment.m_EnableBlend;
				m_PipelineStateDescriptor.BlendState.RenderTarget[i].LogicOpEnable = FALSE;
				m_PipelineStateDescriptor.BlendState.RenderTarget[i].SrcBlend = GetBlend(attachment.m_SrcBlendFactor);
				m_PipelineStateDescriptor.BlendState.RenderTarget[i].DestBlend = GetBlend(attachment.m_DstBlendFactor);
				m_PipelineStateDescriptor.BlendState.RenderTarget[i].BlendOp = GetBlendOperator(attachment.m_BlendOperator);
				m_PipelineStateDescriptor.BlendState.RenderTarget[i].SrcBlendAlpha = GetBlend(attachment.m_SrcAlphaBlendFactor);
				m_PipelineStateDescriptor.BlendState.RenderTarget[i].DestBlendAlpha = GetBlend(attachment.m_DstAlphaBlendFactor);
				m_PipelineStateDescriptor.BlendState.RenderTarget[i].BlendOpAlpha = GetBlendOperator(attachment.m_AlphaBlendOperator);
				m_PipelineStateDescriptor.BlendState.RenderTarget[i].LogicOp = GetColorBlendLogic(m_Specification.m_ColorBlendLogic);
				m_PipelineStateDescriptor.BlendState.RenderTarget[i].RenderTargetWriteMask = GetWriteEnable(attachment.m_ColorWriteMask);
			}

			m_PipelineStateDescriptor.DepthStencilState.DepthEnable = m_Specification.m_EnableDepthTest;
			m_PipelineStateDescriptor.DepthStencilState.DepthWriteMask = m_Specification.m_EnableDepthWrite ? D3D12_DEPTH_WRITE_MASK_ZERO : D3D12_DEPTH_WRITE_MASK_ALL;
			m_PipelineStateDescriptor.DepthStencilState.DepthFunc = GetComparisonFunction(m_Specification.m_DepthCompareLogic);
			m_PipelineStateDescriptor.DepthStencilState.StencilEnable = FALSE;
			// m_PipelineStateDescriptor.DepthStencilState.StencilReadMask;
			// m_PipelineStateDescriptor.DepthStencilState.StencilWriteMask;
			// m_PipelineStateDescriptor.DepthStencilState.FrontFace.StencilDepthFailOp;
			// m_PipelineStateDescriptor.DepthStencilState.FrontFace.StencilFailOp;
			// m_PipelineStateDescriptor.DepthStencilState.FrontFace.StencilFunc;
			// m_PipelineStateDescriptor.DepthStencilState.FrontFace.StencilPassOp;
			// m_PipelineStateDescriptor.DepthStencilState.BackFace.StencilDepthFailOp;
			// m_PipelineStateDescriptor.DepthStencilState.BackFace.StencilFailOp;
			// m_PipelineStateDescriptor.DepthStencilState.BackFace.StencilFunc;
			// m_PipelineStateDescriptor.DepthStencilState.BackFace.StencilPassOp;

			m_PipelineStateDescriptor.SampleMask = UINT_MAX;
			m_PipelineStateDescriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

			const auto& renderTargets = m_pRasterizer->getRenderTargets();
			m_PipelineStateDescriptor.NumRenderTargets = static_cast<UINT>(renderTargets.size());
			for (uint8_t i = 0; i < renderTargets.size(); i++)
				m_PipelineStateDescriptor.RTVFormats[i] = m_pDevice->convertFormat(renderTargets[i].getDataFormat());

			m_PipelineStateDescriptor.SampleDesc.Count = 1;
			m_PipelineStateDescriptor.SampleDesc.Quality = 0;
		}
	}
}