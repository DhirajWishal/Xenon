// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Rasterizer.hpp"
#include "Shader.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Pipeline primitive topology.
		 * This describes how the vertexes are connected.
		 */
		enum class PrimitiveTopology : uint8_t
		{
			PointList,
			LineList,
			LineStrip,
			TriangleList,
			TriangleStrip,
			TriangleFan,
			LineListWithAdjacency,
			LineStripWithAdjacency,
			TriangleListWithAdjacency,
			TriangleStripWithAdjacency,
			PatchList
		};

		/**
		 * Pipeline cull mode.
		 */
		enum class CullMode : uint8_t
		{
			None,
			Front,
			Back,
			FrontAndBack
		};

		/**
		 * Pipeline front face.
		 * This defines how to find the front size of a triangle.
		 */
		enum class FrontFace : uint8_t
		{
			// In this mode, the front side of it is facing away from you.
			CounterClockwise,

			// In this mode, the front is facing towards you.
			Clockwise
		};

		/**
		 * Pipeline polygon mode.
		 * This defines how a vertex group. A vertex group is defined by the primitive topology.
		 */
		enum class PolygonMode : uint8_t
		{
			Fill,
			Line,
			Point
		};

		/**
		 * Pipeline color blend logic.
		 * This defined what logic to apply when color blending.
		 */
		enum class ColorBlendLogic : uint8_t
		{
			Clear,
			And,
			AndReverse,
			Copy,
			AndInverted,
			NoOperator,
			XOR,
			OR,
			NOR,
			Equivalent,
			Invert,
			ReverseOR,
			CopyInverted,
			InvertedOR,
			NAND,
			Set
		};

		/**
		 * Pipeline depth compare logic.
		 * This defines what logic to apply when depth comparing.
		 */
		enum class DepthCompareLogic : uint8_t
		{
			Never,
			Less,
			Equal,
			LessOrEqual,
			Greater,
			NotEqual,
			GreaterOrEqual,
			Always
		};

		/**
		 * Color blend factor enum.
		 */
		enum class ColorBlendFactor : uint8_t
		{
			Zero,
			One,
			SourceColor,
			OneMinusSourceColor,
			DestinationColor,
			OneMinusDestinationColor,
			SourceAlpha,
			OneMinusSourceAlpha,
			DestinationAlpha,
			OneMinusDestinationAlpha,
			ConstantColor,
			OneMinusConstantColor,
			ConstantAlpha,
			OneMinusConstantAlpha,
			SourceAlphaSaturate,
			SourceOneColor,
			OneMinusSourceOneColor,
			SourceOneAlpha,
			OneMinusSourceOneAlpha
		};

		/**
		 * Color blend operator enum.
		 */
		enum class ColorBlendOperator : uint8_t
		{
			Add,
			Subtract,
			ReverseSubtract,
			Minimum,
			Maximum,
			Zero,
			Source,
			Destination,
			SourceOver,
			DestinationOver,
			SourceIn,
			DestinationIn,
			SouceOut,
			DestinationOut,
			SourceATOP,
			DestinationATOP,
			XOR,
			Multiply,
			Screen,
			Overlay,
			Darken,
			Lighten,
			ColorDodge,
			ColorBurn,
			HardLight,
			SoftLight,
			Difference,
			Exclusion,
			Invert,
			InvertRGB,
			LinearDodge,
			LinearBurn,
			VividLight,
			LinearLight,
			PinLight,
			HardMix,
			HSLHue,
			HSLSaturation,
			HSLColor,
			HSLLuminosity,
			Plus,
			PlusClamped,
			PlusClampedAlpha,
			PlusDarker,
			Minus,
			MinusClamped,
			Contrast,
			InvertOVG,
			Red,
			Green,
			Blue
		};

		/**
		 * Color write mask enum.
		 */
		enum class ColorWriteMask : uint8_t
		{
			R = XENON_BIT_SHIFT(0),
			G = XENON_BIT_SHIFT(1),
			B = XENON_BIT_SHIFT(2),
			A = XENON_BIT_SHIFT(3)
		};

		XENON_DEFINE_ENUM_AND(ColorWriteMask);
		XENON_DEFINE_ENUM_OR(ColorWriteMask);

		/**
		 * Color blend attachment structure.
		 */
		struct ColorBlendAttachment final
		{
			bool m_EnableBlend = false;
			ColorBlendFactor m_SrcBlendFactor = ColorBlendFactor::Zero;
			ColorBlendFactor m_DstBlendFactor = ColorBlendFactor::Zero;
			ColorBlendFactor m_SrcAlphaBlendFactor = ColorBlendFactor::Zero;
			ColorBlendFactor m_DstAlphaBlendFactor = ColorBlendFactor::Zero;
			ColorBlendOperator m_BlendOperator = ColorBlendOperator::Add;
			ColorBlendOperator m_AlphaBlendOperator = ColorBlendOperator::Add;
			ColorWriteMask m_ColorWriteMask = ColorWriteMask::R | ColorWriteMask::G | ColorWriteMask::B | ColorWriteMask::A;
		};

		/**
		 * Dynamic state flags.
		 * This determines which dynamic states the pipeline consists.
		 */
		enum class DynamicStateFlags : uint8_t
		{
			Undefined,
			LineWidth = XENON_BIT_SHIFT(1),
			DepthBias = XENON_BIT_SHIFT(2),
			BlendConstants = XENON_BIT_SHIFT(3),
			DepthBounds = XENON_BIT_SHIFT(4)
		};

		XENON_DEFINE_ENUM_AND(DynamicStateFlags);
		XENON_DEFINE_ENUM_OR(DynamicStateFlags);

		/**
		 * Rasterizing pipeline specification.
		 */
		struct RasterizingPipelineSpecification final
		{
			Shader m_VertexShader;
			Shader m_FragmentShader;	// AKA the pixel shader.

			std::vector<ColorBlendAttachment> m_ColorBlendAttachments = { ColorBlendAttachment() };

			std::array<float, 4> m_ColorBlendConstants = {};
			float m_DepthBiasFactor = 0.0f;
			float m_DepthConstantFactor = 0.0f;
			float m_DepthSlopeFactor = 0.0f;
			float m_RasterizerLineWidth = 1.0f;
			float m_MinSampleShading = 1.0f;

			uint32_t m_TessellationPatchControlPoints = 0;

			PrimitiveTopology m_PrimitiveTopology = PrimitiveTopology::TriangleList;
			CullMode m_CullMode = CullMode::Back;
			FrontFace m_FrontFace = FrontFace::CounterClockwise;
			PolygonMode m_PolygonMode = PolygonMode::Fill;
			ColorBlendLogic m_ColorBlendLogic = ColorBlendLogic::Clear;
			DepthCompareLogic m_DepthCompareLogic = DepthCompareLogic::LessOrEqual;
			DynamicStateFlags m_DynamicStateFlags = DynamicStateFlags::Undefined;

			bool m_EnablePrimitiveRestart : 1 = false;
			bool m_EnableDepthBias : 1 = false;
			bool m_EnableDepthClamp : 1 = false;
			bool m_EnableRasterizerDiscard : 1 = false;
			bool m_EnableAlphaCoverage : 1 = false;
			bool m_EnableAlphaToOne : 1 = false;
			bool m_EnableSampleShading : 1 = true;
			bool m_EnableColorBlendLogic : 1 = false;
			bool m_EnableDepthTest : 1 = true;
			bool m_EnableDepthWrite : 1 = true;
		};

		/**
		 * Rasterizing pipeline class.
		 * This class is used by a rasterizer to perform rasterization to a set of primitives.
		 */
		class RasterizingPipeline : public Pipeline
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer.
			 * @param pRasterizer The rasterizer pointer.
			 * @param specification The pipeline specification.
			 */
			explicit RasterizingPipeline(const Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, [[maybe_unused]] const Rasterizer* pRasterizer, const RasterizingPipelineSpecification& specification)
				: Pipeline(pDevice, std::move(pCacheHandler)), m_Specification(specification) {}

		protected:
			RasterizingPipelineSpecification m_Specification;
		};
	}
}