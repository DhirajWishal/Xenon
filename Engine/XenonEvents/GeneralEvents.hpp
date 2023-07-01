// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <string>

namespace Xenon
{
	/**
	 * General events structure.
	 * This contains general events that took place in the engine that doesn't need/ have it's own category.
	 */
	struct GeneralEvents final
	{
		GeneralEvents() = default;

		std::vector<std::string> m_DragDropFiles;
	};
}