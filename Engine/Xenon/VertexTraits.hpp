// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"

#include <array>

namespace Xenon
{
	/**
	 * Vertex element enum.
	 * This contains different elements which a single vertex could contain.
	 *
	 * Note that when accessing these elements from a vertex shader, the layout location is the same as the value of the element.
	 * For example, the position values are passed into location layout 0 and color index 0 values are passed into layout location 4.
	 */
	enum class VertexElement : uint8_t
	{
		Position,				// Stored and used as a 3 component vector.
		Normal,					// Stored and used as a 3 component vector.

		Tangent,				// Stored and used as a 3 component vector.
		BiTangent,				// Stored and used as a 3 component vector.

		Color_0,				// Stored and used as a 4 component vector.
		Color_1,				// Stored and used as a 4 component vector.
		Color_2,				// Stored and used as a 4 component vector.
		Color_3,				// Stored and used as a 4 component vector.
		Color_4,				// Stored and used as a 4 component vector.
		Color_5,				// Stored and used as a 4 component vector.
		Color_6,				// Stored and used as a 4 component vector.
		Color_7,				// Stored and used as a 4 component vector.

		TextureCoordinate_0,	// Stored and used as a 2 component vector.
		TextureCoordinate_1,	// Stored and used as a 2 component vector.
		TextureCoordinate_2,	// Stored and used as a 2 component vector.
		TextureCoordinate_3,	// Stored and used as a 2 component vector.
		TextureCoordinate_4,	// Stored and used as a 2 component vector.
		TextureCoordinate_5,	// Stored and used as a 2 component vector.
		TextureCoordinate_6,	// Stored and used as a 2 component vector.
		TextureCoordinate_7,	// Stored and used as a 2 component vector.

		JointIndices,			// Stored and used as a 4 component vector.
		JointWeight,			// Stored and used as a 4 component vector.

		Count,					// This is just a count of the number of supported vertex elements.
		Undefined				// Undefined element.
	};

	/**
	 * Get a vertex element's component count.
	 * Every element has it's own defined component count. The size of the component can differ.
	 *
	 * @param element The element type.
	 * @return The component count of that element.
	 */
	[[nodiscard]] constexpr uint8_t GetVertexElementComponentCount(VertexElement element)
	{
		switch (element)
		{
		case Xenon::VertexElement::Position:
		case Xenon::VertexElement::Normal:
		case Xenon::VertexElement::Tangent:
		case Xenon::VertexElement::BiTangent:
			return 3;

		case Xenon::VertexElement::Color_0:
		case Xenon::VertexElement::Color_1:
		case Xenon::VertexElement::Color_2:
		case Xenon::VertexElement::Color_3:
		case Xenon::VertexElement::Color_4:
		case Xenon::VertexElement::Color_5:
		case Xenon::VertexElement::Color_6:
		case Xenon::VertexElement::Color_7:
			return 4;

		case Xenon::VertexElement::TextureCoordinate_0:
		case Xenon::VertexElement::TextureCoordinate_1:
		case Xenon::VertexElement::TextureCoordinate_2:
		case Xenon::VertexElement::TextureCoordinate_3:
		case Xenon::VertexElement::TextureCoordinate_4:
		case Xenon::VertexElement::TextureCoordinate_5:
		case Xenon::VertexElement::TextureCoordinate_6:
		case Xenon::VertexElement::TextureCoordinate_7:
			return 2;

		case Xenon::VertexElement::JointIndices:
		case Xenon::VertexElement::JointWeight:
			return 4;

		default:
			return 0;
		}
	}

	/**
	 * Vertex specification class.
	 * This contains information about a single vertex including it's size, and the actual elements that are been stored.
	 */
	class VertexSpecification final
	{
	public:
		/**
		 * Default constructor.
		 */
		VertexSpecification() = default;

		/**
		 * Add a vertex element to the specification.
		 *
		 * @param element The element to add.
		 * @param componentSize The size of a single component in the element in bytes. Default is sizeof(float).
		 * @return The specification reference.
		 */
		VertexSpecification& addElement(VertexElement element, uint8_t componentSize = sizeof(float))
		{
			m_VertexElements |= 1 << EnumToInt(element);
			m_ElementSizes[EnumToInt(element)] = componentSize * GetVertexElementComponentCount(element);

			return *this;
		}

		/**
		 * Get the size of the vertex.
		 *
		 * @return The size in bytes.
		 */
		[[nodiscard]] uint32_t getSize() const
		{
			uint32_t size = 0;
			for (const auto elementSize : m_ElementSizes)
				size += elementSize;

			return size;
		}

		/**
		 * Check if a vertex element is present in the vertex.
		 *
		 * @param element The element to check.
		 * @return True if the element is present.
		 * @return False if the element is not present.
		 */
		[[nodiscard]] bool isAvailable(VertexElement element) const { return m_VertexElements & (1 << EnumToInt(element)); }

	private:
		uint32_t m_VertexElements = 0;
		std::array<uint8_t, EnumToInt(VertexElement::Count)> m_ElementSizes = { 0 };
	};
}