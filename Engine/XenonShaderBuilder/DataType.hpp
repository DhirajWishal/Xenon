// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "AssemblyStorage.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Data type class.
		 * This is the base class for all the major wrapper types in the shader builder.
		 */
		class DataType
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the commands to.
			 */
			explicit DataType(AssemblyStorage& storage) : m_Storage(storage), m_Identifier(storage.getUniqueID()) {}

			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the commands to.
			 * @param identifier The data type's identifier.
			 */
			explicit DataType(AssemblyStorage& storage, uint32_t identifier) : m_Storage(storage), m_Identifier(identifier) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~DataType() = default;

			/**
			 * Get the attribute's unique ID.
			 *
			 * @return The identifier.
			 */
			[[nodiscard]] uint32_t getID() const noexcept { return m_Identifier; }

			/**
			 * Access a member of the current type.
			 * This is needed when accessing from a struct or any other complex type.
			 *
			 * @param memberID The member's ID.
			 * @return The accessed ID.
			 */
			[[nodiscard]] virtual uint32_t accessMember(uint32_t memberID) { return memberID; }

		protected:
			AssemblyStorage& m_Storage;
			uint32_t m_Identifier = 0;
		};
	}
}