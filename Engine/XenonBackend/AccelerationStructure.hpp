// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Acceleration structure level enum.
		 */
		enum class AccelerationStructureLevel : uint8_t
		{
			// This contains the geometry instances.
			Top,

			// This contains the actual geometry.
			Bottom
		};

		/**
		 * Acceleration structure class.
		 * This structure is used to store geometry and instance information (bottom level and top level) which is used for ray tracing.
		 */
		class AccelerationStructure : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param level The acceleration structure level.
			 */
			explicit AccelerationStructure([[maybe_unused]] const Device* pDevice, AccelerationStructureLevel level) : m_Level(level) {}

			/**
			 * Get the acceleration structure level.
			 *
			 * @return The acceleration structure level.
			 */
			[[nodiscard]] AccelerationStructureLevel getLevel() const noexcept { return m_Level; }

		private:
			AccelerationStructureLevel m_Level = AccelerationStructureLevel::Top;
		};
	}
}