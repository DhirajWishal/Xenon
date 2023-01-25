// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRenderer.hpp"

namespace Xenon
{
	namespace Experimental
	{
		DefaultRenderer::DefaultRenderer(Instance& instance, Backend::Camera* pCamera, const std::string& title)
			: Renderer(instance, pCamera, title)
		{

		}
	}
}