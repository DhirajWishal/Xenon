// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <typeindex>

namespace Xenon
{
	/**
	 * Utility function to get the type index of a type.
	 *
	 * @tparam Type The type to get the type index of.
	 * @return The type index.
	 */
	template<class Type>
	constexpr std::type_index GetTypeIndex() { return std::type_index(typeid(Type)); }
}