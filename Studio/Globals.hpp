// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Xenon/Instance.hpp"

/**
 * Globals structure.
 * This contains all the global information used by the studio.
 */
struct Globals
{
	Xenon::BackendType m_CurrentBackendType = Xenon::BackendType::Any;
	Xenon::BackendType m_RequiredBackendType = Xenon::BackendType::Any;
	bool m_bExitAppliation = false;
};

inline Globals g_Globals;