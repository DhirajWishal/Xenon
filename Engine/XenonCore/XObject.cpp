// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "XObject.hpp"

namespace Xenon
{
	Xenon::JobSystem& XObject::GetJobSystem()
	{
		static auto jobSystem = JobSystem(std::thread::hardware_concurrency() - 1);	// We leave one here for the parent thread..
		return jobSystem;
	}
}