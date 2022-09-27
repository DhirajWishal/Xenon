// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Instance.hpp"

namespace Xenon
{
	Instance::Instance(const std::string& applicationName, uint32_t applicationVersion, RenderTargetType renderTargets)
		: m_ApplicationName(applicationName)
		, m_ApplicationVersion(applicationVersion)
		, m_RenderTargets(renderTargets)
	{
	}
}