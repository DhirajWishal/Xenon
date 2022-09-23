// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Xenon.hpp"

namespace Xenon
{
	Application::Application(const std::string& name, uint64_t version)
		: m_Name(name), m_Version(version)
	{
	}
}