// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRasterizingPipeline.hpp"
#include "VulkanMacros.hpp"
#include "VulkanRasterizer.hpp"
#include "VulkanDescriptorSetManager.hpp"

#ifdef XENON_PLATFORM_WINDOWS
#include <execution>

#endif

namespace /* anonymous */
{
	/**
	 * Get the shader stage flag bit from the Xenon shader type.
	 *
	 * @param shaderType The shader type.
	 * @return The Vulkan shader stage flag bit.
	 */
	[[nodiscard]] constexpr VkShaderStageFlagBits GetShaderStageFlagBit(Xenon::Backend::ShaderType shaderType) noexcept
	{
		switch (shaderType)
		{
		case Xenon::Backend::ShaderType::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;

		case Xenon::Backend::ShaderType::Fragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;

		case Xenon::Backend::ShaderType::RayGen:
			return VK_SHADER_STAGE_RAYGEN_BIT_KHR;

		case Xenon::Backend::ShaderType::Intersection:
			return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;

		case Xenon::Backend::ShaderType::AnyHit:
			return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

		case Xenon::Backend::ShaderType::ClosestHit:
			return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

		case Xenon::Backend::ShaderType::Miss:
			return VK_SHADER_STAGE_MISS_BIT_KHR;

		case Xenon::Backend::ShaderType::Callable:
			return VK_SHADER_STAGE_CALLABLE_BIT_KHR;

		case Xenon::Backend::ShaderType::Compute:
			return VK_SHADER_STAGE_COMPUTE_BIT;

		default:
			XENON_LOG_ERROR("Invalid shader type provided! Defaulting to All.");
			return VK_SHADER_STAGE_ALL;
		}
	}

	/**
	 * Get the shader bindings.
	 *
	 * @param shader The shader to get the bindings from.
	 * @param bindingMap The binding map contains set-binding info.
	 * @param indexToBindingMap The map to go from the binding index to binding information.
	 * @param type The shader type.
	 */
	void GetShaderBindings(
		const Xenon::Backend::ShaderSource& shader,
		std::unordered_map<uint32_t, std::vector<Xenon::Backend::DescriptorBindingInfo>>& bindingMap,
		std::unordered_map<uint32_t, std::unordered_map<uint32_t, size_t>>& indexToBindingMap,
		std::vector<VkPushConstantRange>& pushConstants,
		std::vector<VkVertexInputBindingDescription>& inputBindingDescriptions,
		std::vector<VkVertexInputAttributeDescription>& inputAttributeDescriptions,
		Xenon::Backend::ShaderType type)
	{
		const auto shaderStage = GetShaderStageFlagBit(type);

		// Get the resources.
		for (const auto& resource : shader.getResources())
		{
			auto& bindings = bindingMap[Xenon::EnumToInt(resource.m_Set)];
			auto& indexToBinding = indexToBindingMap[Xenon::EnumToInt(resource.m_Set)];

			if (indexToBinding.contains(resource.m_Binding))
			{
				bindings[indexToBinding[resource.m_Binding]].m_ApplicableShaders |= type;
			}
			else
			{
				indexToBinding[resource.m_Binding] = bindings.size();
				auto& binding = bindings.emplace_back();
				binding.m_Type = resource.m_Type;
				binding.m_ApplicableShaders = type;
			}
		}

		// Get the buffers.
		for (const auto& buffer : shader.getConstantBuffers())
		{
			auto& range = pushConstants.emplace_back();
			range.offset = buffer.m_Offset;
			range.size = buffer.m_Size;
			range.stageFlags = shaderStage;
		}

		// Setup the input bindings if we're on the vertex shader.
		if (type == Xenon::Backend::ShaderType::Vertex)
		{
			for (const auto& input : shader.getInputAttributes())
			{
				const auto element = static_cast<Xenon::Backend::InputElement>(input.m_Location);
				if (IsVertexElement(element))
					continue;

				auto& attribute = inputAttributeDescriptions.emplace_back();
				attribute.binding = 1;
				attribute.location = input.m_Location;

				switch (element)
				{
				case Xenon::Backend::InputElement::InstancePosition:
					attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
					attribute.offset = offsetof(Xenon::Backend::InstanceEntry, m_Position);
					break;

				case Xenon::Backend::InputElement::InstanceRotation:
					attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
					attribute.offset = offsetof(Xenon::Backend::InstanceEntry, m_Rotation);
					break;

				case Xenon::Backend::InputElement::InstanceScale:
					attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
					attribute.offset = offsetof(Xenon::Backend::InstanceEntry, m_Scale);
					break;

				case Xenon::Backend::InputElement::InstanceID:
					attribute.format = VK_FORMAT_R32_UINT;
					attribute.offset = offsetof(Xenon::Backend::InstanceEntry, m_InstanceID);
					break;

				default:
					break;
				}
			}

			// Setup the binding if we have instance inputs in the shader.
			if (!inputAttributeDescriptions.empty())
			{
				auto& binding = inputBindingDescriptions.emplace_back();
				binding.binding = 1;
				binding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
				binding.stride = sizeof(Xenon::Backend::InstanceEntry);
			}
		}
	}

	/**
	 * Get the primitive topology.
	 *
	 * @param topology The flint primitive topology.
	 * @return The Vulkan primitive topology.
	 */
	[[nodiscard]] constexpr VkPrimitiveTopology GetPrimitiveTopology(Xenon::Backend::PrimitiveTopology topology) noexcept
	{
		switch (topology)
		{
		case Xenon::Backend::PrimitiveTopology::PointList:						return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case Xenon::Backend::PrimitiveTopology::LineList:						return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case Xenon::Backend::PrimitiveTopology::LineStrip:						return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case Xenon::Backend::PrimitiveTopology::TriangleList:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case Xenon::Backend::PrimitiveTopology::TriangleStrip:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case Xenon::Backend::PrimitiveTopology::TriangleFan:					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		case Xenon::Backend::PrimitiveTopology::LineListWithAdjacency:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
		case Xenon::Backend::PrimitiveTopology::LineStripWithAdjacency:			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
		case Xenon::Backend::PrimitiveTopology::TriangleListWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
		case Xenon::Backend::PrimitiveTopology::TriangleStripWithAdjacency:		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		case Xenon::Backend::PrimitiveTopology::PatchList:						return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		default:																XENON_LOG_ERROR("Invalid or Undefined primitive topology!");
		}
	}

	/**
	 * Get the cull mode.
	 *
	 * @param cull The flint cull mode.
	 * @return The Vulkan cull mode.
	 */
	[[nodiscard]] constexpr VkCullModeFlags GetCullMode(Xenon::Backend::CullMode cull) noexcept
	{
		switch (cull)
		{
		case Xenon::Backend::CullMode::None:									return VK_CULL_MODE_NONE;
		case Xenon::Backend::CullMode::Front:									return VK_CULL_MODE_FRONT_BIT;
		case Xenon::Backend::CullMode::Back:									return VK_CULL_MODE_BACK_BIT;
		case Xenon::Backend::CullMode::FrontAndBack:							return VK_CULL_MODE_FRONT_AND_BACK;
		default:																XENON_LOG_ERROR("Invalid or Undefined cull mode!");
		}
	}

	/**
	 * Get the front face.
	 *
	 * @param face The flint front face.
	 * @return The Vulkan front face.
	 */
	[[nodiscard]] constexpr VkFrontFace GetFrontFace(Xenon::Backend::FrontFace face) noexcept
	{
		switch (face)
		{
		case Xenon::Backend::FrontFace::CounterClockwise:						return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case Xenon::Backend::FrontFace::Clockwise:								return VK_FRONT_FACE_CLOCKWISE;
		default:																XENON_LOG_ERROR("Invalid or Undefined front face!");
		}
	}

	/**
	 * Get the polygon mode.
	 *
	 * @param mode The flint polygon mode.
	 * @return The Vulkan polygon mode.
	 */
	[[nodiscard]] constexpr VkPolygonMode GetPolygonMode(Xenon::Backend::PolygonMode mode) noexcept
	{
		switch (mode)
		{
		case Xenon::Backend::PolygonMode::Fill:									return VK_POLYGON_MODE_FILL;
		case Xenon::Backend::PolygonMode::Line:									return VK_POLYGON_MODE_LINE;
		case Xenon::Backend::PolygonMode::Point:								return VK_POLYGON_MODE_POINT;
		default:																XENON_LOG_ERROR("Invalid or Undefined polygon mode!");
		}
	}

	/**
	 * Get the logic operator.
	 *
	 * @param logic The flint logic.
	 * @return The Vulkan logic.
	 */
	[[nodiscard]] constexpr VkLogicOp GetLogicOp(Xenon::Backend::ColorBlendLogic logic) noexcept
	{
		switch (logic)
		{
		case Xenon::Backend::ColorBlendLogic::Clear:							return VK_LOGIC_OP_CLEAR;
		case Xenon::Backend::ColorBlendLogic::And:								return VK_LOGIC_OP_AND;
		case Xenon::Backend::ColorBlendLogic::AndReverse:						return VK_LOGIC_OP_AND_REVERSE;
		case Xenon::Backend::ColorBlendLogic::Copy:								return VK_LOGIC_OP_COPY;
		case Xenon::Backend::ColorBlendLogic::AndInverted:						return VK_LOGIC_OP_AND_INVERTED;
		case Xenon::Backend::ColorBlendLogic::NoOperator:						return VK_LOGIC_OP_NO_OP;
		case Xenon::Backend::ColorBlendLogic::XOR:								return VK_LOGIC_OP_XOR;
		case Xenon::Backend::ColorBlendLogic::OR:								return VK_LOGIC_OP_OR;
		case Xenon::Backend::ColorBlendLogic::NOR:								return VK_LOGIC_OP_NOR;
		case Xenon::Backend::ColorBlendLogic::Equivalent:						return VK_LOGIC_OP_EQUIVALENT;
		case Xenon::Backend::ColorBlendLogic::Invert:							return VK_LOGIC_OP_INVERT;
		case Xenon::Backend::ColorBlendLogic::ReverseOR:						return VK_LOGIC_OP_OR_REVERSE;
		case Xenon::Backend::ColorBlendLogic::CopyInverted:						return VK_LOGIC_OP_COPY_INVERTED;
		case Xenon::Backend::ColorBlendLogic::InvertedOR:						return VK_LOGIC_OP_OR_INVERTED;
		case Xenon::Backend::ColorBlendLogic::NAND:								return VK_LOGIC_OP_NAND;
		case Xenon::Backend::ColorBlendLogic::Set:								return VK_LOGIC_OP_SET;
		default:																XENON_LOG_ERROR("Invalid or Undefined color blend logic!");
		}
	}

	/**
	 * Get the compare operator.
	 *
	 * @param logic the flint logic.
	 * @return The Vulkan logic operator.
	 */
	[[nodiscard]] constexpr VkCompareOp GetCompareOp(Xenon::Backend::DepthCompareLogic logic) noexcept
	{
		switch (logic)
		{
		case Xenon::Backend::DepthCompareLogic::Never:							return VK_COMPARE_OP_NEVER;
		case Xenon::Backend::DepthCompareLogic::Less:							return VK_COMPARE_OP_LESS;
		case Xenon::Backend::DepthCompareLogic::Equal:							return VK_COMPARE_OP_EQUAL;
		case Xenon::Backend::DepthCompareLogic::LessOrEqual:					return VK_COMPARE_OP_LESS_OR_EQUAL;
		case Xenon::Backend::DepthCompareLogic::Greater:						return VK_COMPARE_OP_GREATER;
		case Xenon::Backend::DepthCompareLogic::NotEqual:						return VK_COMPARE_OP_NOT_EQUAL;
		case Xenon::Backend::DepthCompareLogic::GreaterOrEqual:					return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case Xenon::Backend::DepthCompareLogic::Always:							return VK_COMPARE_OP_ALWAYS;
		default:																XENON_LOG_ERROR("Invalid or Undefined depth compare logic!");
		}
	}

	/**
	 * Get the dynamic states.
	 *
	 * @param flags The flint flags.
	 * @return The Vulkan flags.
	 */
	[[nodiscard]] constexpr std::vector<VkDynamicState> GetDynamicStates(Xenon::Backend::DynamicStateFlags flags) noexcept
	{
		std::vector<VkDynamicState> states;
		if (flags & Xenon::Backend::DynamicStateFlags::ViewPort) states.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		if (flags & Xenon::Backend::DynamicStateFlags::Scissor) states.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
		if (flags & Xenon::Backend::DynamicStateFlags::LineWidth) states.emplace_back(VK_DYNAMIC_STATE_LINE_WIDTH);
		if (flags & Xenon::Backend::DynamicStateFlags::DepthBias) states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
		if (flags & Xenon::Backend::DynamicStateFlags::BlendConstants) states.emplace_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
		if (flags & Xenon::Backend::DynamicStateFlags::DepthBounds) states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);

		return states;
	}

	/**
	 * Get the blend factor.
	 *
	 * @param factor The flint factor.
	 * @return The Vulkan factor.
	 */
	[[nodiscard]] constexpr VkBlendFactor GetBlendFactor(Xenon::Backend::ColorBlendFactor factor) noexcept
	{
		switch (factor)
		{
		case Xenon::Backend::ColorBlendFactor::Zero:							return VK_BLEND_FACTOR_ZERO;
		case Xenon::Backend::ColorBlendFactor::One:								return VK_BLEND_FACTOR_ONE;
		case Xenon::Backend::ColorBlendFactor::SourceColor:						return VK_BLEND_FACTOR_SRC_COLOR;
		case Xenon::Backend::ColorBlendFactor::OneMinusSourceColor:				return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case Xenon::Backend::ColorBlendFactor::DestinationColor:				return VK_BLEND_FACTOR_DST_COLOR;
		case Xenon::Backend::ColorBlendFactor::OneMinusDestinationColor:		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case Xenon::Backend::ColorBlendFactor::SourceAlpha:						return VK_BLEND_FACTOR_SRC_ALPHA;
		case Xenon::Backend::ColorBlendFactor::OneMinusSourceAlpha:				return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case Xenon::Backend::ColorBlendFactor::DestinationAlpha:				return VK_BLEND_FACTOR_DST_ALPHA;
		case Xenon::Backend::ColorBlendFactor::OneMinusDestinationAlpha:		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case Xenon::Backend::ColorBlendFactor::ConstantColor:					return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case Xenon::Backend::ColorBlendFactor::OneMinusConstantColor:			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		case Xenon::Backend::ColorBlendFactor::ConstantAlpha:					return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case Xenon::Backend::ColorBlendFactor::OneMinusConstantAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		case Xenon::Backend::ColorBlendFactor::SourceAlphaSaturate:				return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		case Xenon::Backend::ColorBlendFactor::SourceOneColor:					return VK_BLEND_FACTOR_SRC1_COLOR;
		case Xenon::Backend::ColorBlendFactor::OneMinusSourceOneColor:			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		case Xenon::Backend::ColorBlendFactor::SourceOneAlpha:					return VK_BLEND_FACTOR_SRC1_ALPHA;
		case Xenon::Backend::ColorBlendFactor::OneMinusSourceOneAlpha:			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		default:																XENON_LOG_ERROR("Invalid color blend factor!");
		}
	}

	/**
	 * Get the blend operator.
	 *
	 * @param op The flint operator.
	 * @return The Vulkan blend operator.
	 */
	[[nodiscard]] constexpr VkBlendOp GetBlendOp(Xenon::Backend::ColorBlendOperator op) noexcept
	{
		switch (op)
		{
		case Xenon::Backend::ColorBlendOperator::Add:							return VK_BLEND_OP_ADD;
		case Xenon::Backend::ColorBlendOperator::Subtract:						return VK_BLEND_OP_SUBTRACT;
		case Xenon::Backend::ColorBlendOperator::ReverseSubtract:				return VK_BLEND_OP_REVERSE_SUBTRACT;
		case Xenon::Backend::ColorBlendOperator::Minimum:						return VK_BLEND_OP_MIN;
		case Xenon::Backend::ColorBlendOperator::Maximum:						return VK_BLEND_OP_MAX;
		case Xenon::Backend::ColorBlendOperator::Zero:							return VK_BLEND_OP_ZERO_EXT;
		case Xenon::Backend::ColorBlendOperator::Source:						return VK_BLEND_OP_SRC_EXT;
		case Xenon::Backend::ColorBlendOperator::Destination:					return VK_BLEND_OP_DST_EXT;
		case Xenon::Backend::ColorBlendOperator::SourceOver:					return VK_BLEND_OP_SRC_OVER_EXT;
		case Xenon::Backend::ColorBlendOperator::DestinationOver:				return VK_BLEND_OP_DST_OVER_EXT;
		case Xenon::Backend::ColorBlendOperator::SourceIn:						return VK_BLEND_OP_SRC_IN_EXT;
		case Xenon::Backend::ColorBlendOperator::DestinationIn:					return VK_BLEND_OP_DST_IN_EXT;
		case Xenon::Backend::ColorBlendOperator::SouceOut:						return VK_BLEND_OP_SRC_OUT_EXT;
		case Xenon::Backend::ColorBlendOperator::DestinationOut:				return VK_BLEND_OP_DST_OUT_EXT;
		case Xenon::Backend::ColorBlendOperator::SourceATOP:					return VK_BLEND_OP_SRC_ATOP_EXT;
		case Xenon::Backend::ColorBlendOperator::DestinationATOP:				return VK_BLEND_OP_DST_ATOP_EXT;
		case Xenon::Backend::ColorBlendOperator::XOR:							return VK_BLEND_OP_XOR_EXT;
		case Xenon::Backend::ColorBlendOperator::Multiply:						return VK_BLEND_OP_MULTIPLY_EXT;
		case Xenon::Backend::ColorBlendOperator::Screen:						return VK_BLEND_OP_SCREEN_EXT;
		case Xenon::Backend::ColorBlendOperator::Overlay:						return VK_BLEND_OP_OVERLAY_EXT;
		case Xenon::Backend::ColorBlendOperator::Darken:						return VK_BLEND_OP_DARKEN_EXT;
		case Xenon::Backend::ColorBlendOperator::Lighten:						return VK_BLEND_OP_LIGHTEN_EXT;
		case Xenon::Backend::ColorBlendOperator::ColorDodge:					return VK_BLEND_OP_COLORDODGE_EXT;
		case Xenon::Backend::ColorBlendOperator::ColorBurn:						return VK_BLEND_OP_COLORBURN_EXT;
		case Xenon::Backend::ColorBlendOperator::HardLight:						return VK_BLEND_OP_HARDLIGHT_EXT;
		case Xenon::Backend::ColorBlendOperator::SoftLight:						return VK_BLEND_OP_SOFTLIGHT_EXT;
		case Xenon::Backend::ColorBlendOperator::Difference:					return VK_BLEND_OP_DIFFERENCE_EXT;
		case Xenon::Backend::ColorBlendOperator::Exclusion:						return VK_BLEND_OP_EXCLUSION_EXT;
		case Xenon::Backend::ColorBlendOperator::Invert:						return VK_BLEND_OP_INVERT_EXT;
		case Xenon::Backend::ColorBlendOperator::InvertRGB:						return VK_BLEND_OP_INVERT_RGB_EXT;
		case Xenon::Backend::ColorBlendOperator::LinearDodge:					return VK_BLEND_OP_LINEARDODGE_EXT;
		case Xenon::Backend::ColorBlendOperator::LinearBurn:					return VK_BLEND_OP_LINEARBURN_EXT;
		case Xenon::Backend::ColorBlendOperator::VividLight:					return VK_BLEND_OP_VIVIDLIGHT_EXT;
		case Xenon::Backend::ColorBlendOperator::LinearLight:					return VK_BLEND_OP_LINEARLIGHT_EXT;
		case Xenon::Backend::ColorBlendOperator::PinLight:						return VK_BLEND_OP_PINLIGHT_EXT;
		case Xenon::Backend::ColorBlendOperator::HardMix:						return VK_BLEND_OP_HARDMIX_EXT;
		case Xenon::Backend::ColorBlendOperator::HSLHue:						return VK_BLEND_OP_HSL_HUE_EXT;
		case Xenon::Backend::ColorBlendOperator::HSLSaturation:					return VK_BLEND_OP_HSL_SATURATION_EXT;
		case Xenon::Backend::ColorBlendOperator::HSLColor:						return VK_BLEND_OP_HSL_COLOR_EXT;
		case Xenon::Backend::ColorBlendOperator::HSLLuminosity:					return VK_BLEND_OP_HSL_LUMINOSITY_EXT;
		case Xenon::Backend::ColorBlendOperator::Plus:							return VK_BLEND_OP_PLUS_EXT;
		case Xenon::Backend::ColorBlendOperator::PlusClamped:					return VK_BLEND_OP_PLUS_CLAMPED_EXT;
		case Xenon::Backend::ColorBlendOperator::PlusClampedAlpha:				return VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT;
		case Xenon::Backend::ColorBlendOperator::PlusDarker:					return VK_BLEND_OP_PLUS_DARKER_EXT;
		case Xenon::Backend::ColorBlendOperator::Minus:							return VK_BLEND_OP_MINUS_EXT;
		case Xenon::Backend::ColorBlendOperator::MinusClamped:					return VK_BLEND_OP_MINUS_CLAMPED_EXT;
		case Xenon::Backend::ColorBlendOperator::Contrast:						return VK_BLEND_OP_CONTRAST_EXT;
		case Xenon::Backend::ColorBlendOperator::InvertOVG:						return VK_BLEND_OP_INVERT_OVG_EXT;
		case Xenon::Backend::ColorBlendOperator::Red:							return VK_BLEND_OP_RED_EXT;
		case Xenon::Backend::ColorBlendOperator::Green:							return VK_BLEND_OP_GREEN_EXT;
		case Xenon::Backend::ColorBlendOperator::Blue:							return VK_BLEND_OP_BLUE_EXT;
		default:																XENON_LOG_ERROR("Invalid color blend operator!");
		}
	}

	/**
	 * Get the color component flags.
	 *
	 * @param mask The color write mask.
	 * @return The VUlkan mask.
	 */
	[[nodiscard]] constexpr VkColorComponentFlags GetComponentFlags(Xenon::Backend::ColorWriteMask mask) noexcept
	{
		VkColorComponentFlags flags = 0;
		if (mask & Xenon::Backend::ColorWriteMask::R) flags |= VK_COLOR_COMPONENT_R_BIT;
		if (mask & Xenon::Backend::ColorWriteMask::G) flags |= VK_COLOR_COMPONENT_G_BIT;
		if (mask & Xenon::Backend::ColorWriteMask::B) flags |= VK_COLOR_COMPONENT_B_BIT;
		if (mask & Xenon::Backend::ColorWriteMask::A) flags |= VK_COLOR_COMPONENT_A_BIT;

		return flags;
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanRasterizingPipeline::VulkanRasterizingPipeline(VulkanDevice* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, VulkanRasterizer* pRasterizer, const RasterizingPipelineSpecification& specification)
			: RasterizingPipeline(pDevice, std::move(pCacheHandler), pRasterizer, specification)
			, VulkanDeviceBoundObject(pDevice)
			, m_pRasterizer(pRasterizer)
		{
			// Get the shader information.
			std::unordered_map<uint32_t, std::vector<DescriptorBindingInfo>> bindingMap;
			std::unordered_map<uint32_t, std::unordered_map<uint32_t, size_t>> indexToBindingMap;
			std::vector<VkPushConstantRange> pushConstants;

			if (specification.m_VertexShader.isValid())
				GetShaderBindings(specification.m_VertexShader, bindingMap, indexToBindingMap, pushConstants, m_VertexInputBindings, m_VertexInputAttributes, ShaderType::Vertex);

			if (specification.m_FragmentShader.isValid())
				GetShaderBindings(specification.m_FragmentShader, bindingMap, indexToBindingMap, pushConstants, m_VertexInputBindings, m_VertexInputAttributes, ShaderType::Fragment);

			// Get the layouts.
			std::vector<VkDescriptorSetLayout> layouts;
			for (const auto& [set, bindings] : bindingMap)
				layouts.emplace_back(pDevice->getDescriptorSetManager()->getDescriptorSetLayout(bindings, static_cast<DescriptorType>(set)));

			// Create the pipeline layout.
			createPipelineLayout(std::move(layouts), std::move(pushConstants));

			// Setup the initial pipeline data.
			setupPipelineInfo();
		}

		VulkanRasterizingPipeline::~VulkanRasterizingPipeline()
		{
			try
			{
				m_pDevice->getInstance()->getDeletionQueue().insert([pDevice = m_pDevice, layout = m_PipelineLayout]
					{
						pDevice->getDeviceTable().vkDestroyPipelineLayout(pDevice->getLogicalDevice(), layout, nullptr);
					}
				);
			}
			catch (...)
			{
				XENON_VK_ASSERT(VK_ERROR_UNKNOWN, "Failed to push the rasterizing pipeline deletion function to the deletion queue!");
			}
		}

		void VulkanRasterizingPipeline::setup(const VertexSpecification& vertexSpecification)
		{
			const auto hash = GenerateHashFor(vertexSpecification);

			if (!m_Pipelines.contains(hash))
			{
				auto& pipeline = m_Pipelines[hash];
				loadPipelineCache(hash, pipeline);

				// Do the pipeline creation part here.

				savePipelineCache(hash, pipeline);
			}
		}

		void VulkanRasterizingPipeline::createPipelineLayout(std::vector<VkDescriptorSetLayout>&& layouts, std::vector<VkPushConstantRange>&& pushConstantRanges)
		{
			VkPipelineLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
			createInfo.pSetLayouts = layouts.data();
			createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
			createInfo.pPushConstantRanges = pushConstantRanges.data();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreatePipelineLayout(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_PipelineLayout), "Failed to create the pipeline layout!");
		}

		void VulkanRasterizingPipeline::loadPipelineCache(uint64_t hash, PipelineStorage& pipeline) const
		{
			std::vector<std::byte> cacheData;
			if (m_pCacheHandler)
				cacheData = m_pCacheHandler->load(hash);

			else
				XENON_LOG_INFORMATION("A pipeline cache handler was not set to load the pipeline cache.");

			VkPipelineCacheCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.initialDataSize = cacheData.size();
			createInfo.pInitialData = cacheData.data();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreatePipelineCache(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &pipeline.m_PipelineCache), "Failed to load the pipeline cache!");
		}

		void VulkanRasterizingPipeline::savePipelineCache(uint64_t hash, PipelineStorage& pipeline) const
		{
			if (m_pCacheHandler)
			{
				size_t cacheSize = 0;
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkGetPipelineCacheData(m_pDevice->getLogicalDevice(), pipeline.m_PipelineCache, &cacheSize, nullptr), "Failed to get the pipeline cache size!");

				auto cacheData = std::vector<std::byte>(cacheSize);
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkGetPipelineCacheData(m_pDevice->getLogicalDevice(), pipeline.m_PipelineCache, &cacheSize, cacheData.data()), "Failed to get the pipeline cache data!");

				m_pCacheHandler->store(hash, cacheData);
			}
			else
			{
				XENON_LOG_INFORMATION("A pipeline cache handler was not set to save the pipeline cache.");
			}
		}

		void VulkanRasterizingPipeline::setupPipelineInfo()
		{
			// Input assembly state.
			m_InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			m_InputAssemblyStateCreateInfo.pNext = nullptr;
			m_InputAssemblyStateCreateInfo.flags = 0;
			m_InputAssemblyStateCreateInfo.primitiveRestartEnable = XENON_VK_BOOL(m_Specification.m_EnablePrimitiveRestart);
			m_InputAssemblyStateCreateInfo.topology = GetPrimitiveTopology(m_Specification.m_PrimitiveTopology);

			// Tessellation state.
			m_TessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			m_TessellationStateCreateInfo.pNext = nullptr;
			m_TessellationStateCreateInfo.flags = 0;
			m_TessellationStateCreateInfo.patchControlPoints = m_Specification.m_TessellationPatchControlPoints;

			// Color blend state.
			for (const auto& attachment : m_Specification.m_ColorBlendAttachments)
			{
				auto& vAttachmentState = m_CBASS.emplace_back();
				vAttachmentState.blendEnable = XENON_VK_BOOL(attachment.m_EnableBlend);
				vAttachmentState.alphaBlendOp = GetBlendOp(attachment.m_AlphaBlendOperator);
				vAttachmentState.colorBlendOp = GetBlendOp(attachment.m_BlendOperator);
				vAttachmentState.colorWriteMask = GetComponentFlags(attachment.m_ColorWriteMask);
				vAttachmentState.srcColorBlendFactor = GetBlendFactor(attachment.m_SrcBlendFactor);
				vAttachmentState.srcAlphaBlendFactor = GetBlendFactor(attachment.m_SrcAlphaBlendFactor);
				vAttachmentState.dstAlphaBlendFactor = GetBlendFactor(attachment.m_DstAlphaBlendFactor);
				vAttachmentState.dstColorBlendFactor = GetBlendFactor(attachment.m_DstBlendFactor);
			}

			m_ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			m_ColorBlendStateCreateInfo.pNext = nullptr;
			m_ColorBlendStateCreateInfo.flags = 0;
			m_ColorBlendStateCreateInfo.logicOp = GetLogicOp(m_Specification.m_ColorBlendLogic);
			m_ColorBlendStateCreateInfo.logicOpEnable = XENON_VK_BOOL(m_Specification.m_EnableColorBlendLogic);

#ifdef XENON_PLATFORM_WINDOWS
			std::copy_n(std::execution::unseq, m_Specification.m_ColorBlendConstants, 4, m_ColorBlendStateCreateInfo.blendConstants);

#else 
			std::copy_n(m_Specification.m_ColorBlendConstants, 4, m_ColorBlendStateCreateInfo.blendConstants);

#endif

			m_ColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(m_CBASS.size());
			m_ColorBlendStateCreateInfo.pAttachments = m_CBASS.data();

			// Rasterization state.
			m_RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			m_RasterizationStateCreateInfo.pNext = nullptr;
			m_RasterizationStateCreateInfo.flags = 0;
			m_RasterizationStateCreateInfo.cullMode = GetCullMode(m_Specification.m_CullMode);
			m_RasterizationStateCreateInfo.depthBiasEnable = XENON_VK_BOOL(m_Specification.m_EnableDepthBias);
			m_RasterizationStateCreateInfo.depthBiasClamp = m_Specification.m_DepthBiasFactor;
			m_RasterizationStateCreateInfo.depthBiasConstantFactor = m_Specification.m_DepthConstantFactor;
			m_RasterizationStateCreateInfo.depthBiasSlopeFactor = m_Specification.m_DepthSlopeFactor;
			m_RasterizationStateCreateInfo.depthClampEnable = XENON_VK_BOOL(m_Specification.m_EnableDepthClamp);
			m_RasterizationStateCreateInfo.frontFace = GetFrontFace(m_Specification.m_FrontFace);
			m_RasterizationStateCreateInfo.lineWidth = m_Specification.m_RasterizerLineWidth;
			m_RasterizationStateCreateInfo.polygonMode = GetPolygonMode(m_Specification.m_PolygonMode);
			m_RasterizationStateCreateInfo.rasterizerDiscardEnable = XENON_VK_BOOL(m_Specification.m_EnableRasterizerDiscard);

			// Multisample state.
			m_MultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			m_MultisampleStateCreateInfo.pNext = nullptr;
			m_MultisampleStateCreateInfo.flags = 0;
			m_MultisampleStateCreateInfo.alphaToCoverageEnable = XENON_VK_BOOL(m_Specification.m_EnableAlphaCoverage);
			m_MultisampleStateCreateInfo.alphaToOneEnable = XENON_VK_BOOL(m_Specification.m_EnableAlphaToOne);
			m_MultisampleStateCreateInfo.minSampleShading = m_Specification.m_MinSampleShading;
			m_MultisampleStateCreateInfo.pSampleMask = nullptr;	// TODO
			m_MultisampleStateCreateInfo.rasterizationSamples = m_pDevice->convertSamplingCount(m_pRasterizer->getImageAttachment(AttachmentType::Color)->getSpecification().m_MultiSamplingCount);
			m_MultisampleStateCreateInfo.sampleShadingEnable = XENON_VK_BOOL(m_Specification.m_EnableSampleShading);

			// Depth stencil state.
			m_DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			m_DepthStencilStateCreateInfo.pNext = nullptr;
			m_DepthStencilStateCreateInfo.flags = 0;
			m_DepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			m_DepthStencilStateCreateInfo.depthTestEnable = XENON_VK_BOOL(m_Specification.m_EnableDepthTest);
			m_DepthStencilStateCreateInfo.depthWriteEnable = XENON_VK_BOOL(m_Specification.m_EnableDepthWrite);
			m_DepthStencilStateCreateInfo.depthCompareOp = GetCompareOp(m_Specification.m_DepthCompareLogic);

			// Dynamic state.
			m_DynamicStates = std::move(GetDynamicStates(m_Specification.m_DynamicStateFlags));

			m_DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			m_DynamicStateCreateInfo.pNext = VK_NULL_HANDLE;
			m_DynamicStateCreateInfo.flags = 0;
			m_DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
			m_DynamicStateCreateInfo.pDynamicStates = m_DynamicStates.data();
		}
	}
}