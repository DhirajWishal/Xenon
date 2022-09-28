// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"

#include "Instance.hpp"

namespace Xenon
{
	/**
	 * Render target type enum.
	 */
	enum class RenderTargetType : uint8_t
	{
		Rasterizer = XENON_BIT_SHIFT(0),
		RayTracer = XENON_BIT_SHIFT(1),
		PathTracer = XENON_BIT_SHIFT(2),

		All = Rasterizer | RayTracer | PathTracer
	};

	XENON_DEFINE_ENUM_OR(RenderTargetType);
	XENON_DEFINE_ENUM_AND(RenderTargetType);

	namespace Backend
	{
		/**
		 * Device class.
		 * This represents information about a single GPU.
		 */
		class Device : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pInstance The instance pointer.
			 * @param requiredRenderTargets The render targets the device must support.
			 */
			explicit Device(Instance* pInstance, RenderTargetType requiredRenderTargets) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Device() = default;
		};
	}
}