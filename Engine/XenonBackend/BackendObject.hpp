// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"
#include "../XenonCore/XObject.hpp"

#include <utility>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Backend object class.
		 * This is the base class for all the backend objects.
		 */
		class BackendObject : public XObject
		{
		public:
			/**
			 * Default constructor.
			 */
			BackendObject() = default;

			/**
			 * Move constructor.
			 *
			 * @param other The other constructor.
			 */
			BackendObject(BackendObject&& other) noexcept : m_IsValid(std::exchange(other.m_IsValid, false)) {}

			// Disable copy for the object.
			XENON_DISABLE_COPY(BackendObject);

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

			/**
			 * Check if the object is valid or not.
			 * Invalid objects are not recommended to have around. But they might be the result of a move operation.
			 *
			 * @return True if the object is valid.
			 * @return False if the object is invalid.
			 */
			[[nodiscard]] bool isValid() const { return m_IsValid; }

		public:
			/**
			 * Move assignment operator.
			 *
			 * @param other The other object.
			 * @return The moved object.
			 */
			BackendObject& operator=(BackendObject&& other) noexcept { m_IsValid = std::exchange(other.m_IsValid, false); return *this; }

		protected:
			bool m_IsValid = true;
		};
	}
}