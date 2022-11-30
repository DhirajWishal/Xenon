// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GlobalEvents.hpp"

namespace Xenon
{
	Xenon::GlobalEvents& GlobalEvents::Get()
	{
		static GlobalEvents instance;
		return instance;
	}
}