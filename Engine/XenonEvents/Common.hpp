// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"

namespace Xenon
{
	/**
	 * Button event enum.
	 */
	enum class ButtonEvent : uint8_t
	{
		Release,
		Pressed,
		Repeat
	};
}