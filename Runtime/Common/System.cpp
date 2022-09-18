// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "System.hpp"

namespace Xenon
{
	void System::handleRequests()
	{
		const auto lock = std::scoped_lock(m_RequestMutex);
		for (auto& request : m_Requests)
			request();

		m_Requests.clear();
	}
}
