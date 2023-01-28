// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LightLUT.hpp"

#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

namespace Xenon
{
	namespace Experimental
	{
		LightLUT::LightLUT(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority)
			: Layer(renderer, priority)
		{

		}

		void LightLUT::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{

		}
	}
}