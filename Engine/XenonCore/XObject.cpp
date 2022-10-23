// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "XObject.hpp"

namespace Xenon
{
	Xenon::JobSystem XObject::m_JobSystem = JobSystem(std::thread::hardware_concurrency());
}