// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <type_traits>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Backend object class.
		 * This is the base class for all the backend objects.
		 */
		class BackendObject
		{
		public:
			/**
			 * Default constructor.
			 */
			BackendObject() = default;

			/**
			 * Default virtual destructor.
			 */
			virtual ~BackendObject() = default;

			/**
			 * Get this object casted to another type.
			 * Make sure that the casting type is inherited from this class.
			 *
			 * @tparam Type The type to cast to.
			 * @return The casted type pointer.
			 */
			template<class Type>
			[[nodiscard]] Type* as()
			{
				static_assert(std::is_base_of_v<BackendObject, Type>, "Invalid type cast! Make sure that the type to cast to is inherited from this class.");
				return static_cast<Type*>(this);
			}

			/**
			 * Get this object casted to another type.
			 * Make sure that the casting type is inherited from this class.
			 *
			 * @tparam Type The type to cast to.
			 * @return The casted const type pointer.
			 */
			template<class Type>
			[[nodiscard]] const Type* as() const
			{
				static_assert(std::is_base_of_v<BackendObject, Type>, "Invalid type cast! Make sure that the type to cast to is inherited from this class.");
				return static_cast<const Type*>(this);
			}
		};
	}
}