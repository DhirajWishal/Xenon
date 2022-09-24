// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <typeindex>
#include <chrono>

#define XENON_DISABLE_COPY(object)		object(const object&) = delete; object& operator=(const object&) = delete
#define XENON_DISABLE_MOVE(object)		object(object&&) = delete; object& operator=(object&&) = delete 

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

	/**
	 * This type is used as the delta time type by the engine.
	 */
	using DeltaTime = std::chrono::microseconds;
}