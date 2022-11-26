// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Core.hpp"

#include <array>

namespace Xenon
{
	using Backend::VertexElement;

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
		constexpr VertexSpecification() = default;

		/**
		 * Add a vertex element to the specification.
		 *
		 * @param element The element to add.
		 * @param componentSize The size of a single component in the element in bytes. Default is sizeof(float).
		 * @return The specification reference.
		 */
		VertexSpecification& addElement(VertexElement element, uint8_t componentSize = sizeof(float))
		{
			const auto size = componentSize * GetVertexElementComponentCount(element);

			// Update the information only if it's needed.
			if (!isAvailable(element) || m_ElementSizes[EnumToInt(element)] != size)
			{
				m_VertexElements |= 1 << EnumToInt(element);
				m_ElementSizes[EnumToInt(element)] = size;
			}

			return *this;
		}

		/**
		 * Get the size of a single element.
		 *
		 * @param element The element to get the size of.
		 * @return The element's size.
		 */
		[[nodiscard]] uint8_t getElementSize(VertexElement element) const { return m_ElementSizes[EnumToInt(element)]; }

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