// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Uniform.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Buffer type.
		 * This is a uniform type which can store (almost) any user defined structure as a buffer.
		 */
		template<class Type>
		class Buffer : public Uniform<Buffer<Type>>
		{
			using Super = Uniform<Buffer<Type>>;

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 * @param set The descriptor set.
			 * @param binding The uniform binding.
			 */
			template<class... MemberTypes>
			explicit Buffer(AssemblyStorage& storage, uint32_t set, uint32_t binding, const MemberTypes&... members) : Uniform<Buffer<Type>>(storage, set, binding)
			{
				storage.registerObject<Type>(members...);
				storage.insertAnnotation(fmt::format("OpDecorate {} Block", storage.getObjectIdentifier<Type>()));

				storage.insertType(fmt::format("%uniform_pointer_{} = OpTypePointer Uniform {}", Super::m_Identifier, storage.getObjectIdentifier<Type>()));
				storage.insertType(fmt::format("%{} = OpVariable %uniform_pointer_{} Uniform", Super::m_Identifier, Super::m_Identifier));
			}

			/**
			 * Access a member from the buffer.
			 *
			 * @tparam ValueType The value type.
			 * @param member The member variable pointer.
			 * @return The value type reference.
			 */
			template<class ValueType>
			[[nodiscard]] ValueType& access(const ValueType(Type::* member))
			{
				return (static_cast<Type*>(this)->*member);
			}
		};
	}
}