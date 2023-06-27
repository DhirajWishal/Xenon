// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "UniformGridRenderer.hpp"

namespace Xenon
{
	UniformGridRenderer::UniformGridRenderer(Instance& instance, uint32_t width, uint32_t height, const std::string& title)
		: Renderer(instance, width, height, title)
	{
		// Steps:
		// 1. Generate BLAS for each model.
		// 2. Generate TLAS for the scene.
		// 3. Generate output image (color image).
		// 4. Perform ray tracing on each pixel using compute shaders.
		// 5. Present the image to the user.
	}
}