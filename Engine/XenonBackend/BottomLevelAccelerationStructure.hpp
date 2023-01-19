// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "AccelerationStructure.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Acceleration structure geometry structure.
		 */
		struct AccelerationStructureGeometry final
		{
			VertexSpecification m_VertexSpecification = {};

			Buffer* m_pVertexBuffer = nullptr;
			Buffer* m_pIndexBuffer = nullptr;

			IndexBufferStride m_IndexBufferStride = IndexBufferStride::Uint16;
		};

		/**
		 * Bottom level acceleration structure class.
		 * This structure contains the actual geometry data.
		 */
		class BottomLevelAccelerationStructure : public AccelerationStructure
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param geometries The geometries to be stored in the acceleration structure.
			 */
			explicit BottomLevelAccelerationStructure(const Device* pDevice, [[maybe_unused]] const std::vector<AccelerationStructureGeometry>& geometries) : AccelerationStructure(pDevice) {}
		};
	}
}