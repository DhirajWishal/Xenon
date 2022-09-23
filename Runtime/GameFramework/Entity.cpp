// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Entity.hpp"

namespace Xenon
{
	JobSystem& Entity::getJobSystem()
	{
		static JobSystem instance = JobSystem(std::thread::hardware_concurrency());
		return instance;
	}
}
