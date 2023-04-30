// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "StudioConfiguration.hpp"

constexpr auto g_BackendTypeKey = "backendType";

Xenon::BackendType StudioConfiguration::getCurrentBackendType() const
{
	if (m_Document.contains(g_BackendTypeKey))
		return static_cast<Xenon::BackendType>(m_Document[g_BackendTypeKey]);

	return Xenon::BackendType::Any;
}

void StudioConfiguration::setCurrentBackendType(Xenon::BackendType type)
{
	m_Document[g_BackendTypeKey] = Xenon::EnumToInt(type);
}