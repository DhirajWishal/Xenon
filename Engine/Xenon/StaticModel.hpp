// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

namespace Xenon
{
	/**
	 * Vertex mode enum.
	 * This enum contains the two main modes of storing vertex data.
	 * Note that this might impact performance hits on larger models and on different platforms/ GPUs.
	 */
	enum class VertexMode : uint8_t
	{
		// Structure Of Arrays.
		// In this mode, all the individual vertex data (position, normal, color, texture coordinate, etc...) are stored in individual buffers
		// as opposed to storing everything in a single buffer (interleaving).
		// 
		// It may look like this while in the container:
		// Position Vertex Buffer: [PPP][PPP][PPP][PPP]
		// Normal Vertex Buffer: [NNN][NNN][NNN][NNN]
		// Color Vertex Buffer: [CCCC][CCCC][CCCC][CCCC]
		// Texture Coordinate Vertex Buffer: [TT][TT][TT][TT]
		SoA,

		// Array of Structures.
		// In this mode, all the vertex data (position, normal, color, texture coordinate, etc...) are stored in a single buffer by interleaving them.
		// 
		// It may look like this while in the vertex buffer:
		// Vertex Buffer: [PPPNNNCCCCTT][PPPNNNCCCCTT][PPPNNNCCCCTT][PPPNNNCCCCTT]
		AoS
	};

	class StaticModel final
	{
	public:
		explicit StaticModel(VertexMode mode = VertexMode::SoA) : m_VertexMode(mode) {}

	private:
		VertexMode m_VertexMode = VertexMode::SoA;
	};
}