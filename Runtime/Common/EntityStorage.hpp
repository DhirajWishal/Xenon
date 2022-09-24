// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SparseArray.hpp"
#include "Common.hpp"

#include <unordered_map>

namespace Xenon
{
	/**
	 * Interface entity container class.
	 * This class is the base class for the entity container which actually holds the entities.
	 */
	class IEntityContainer {};

	/**
	 * Entity container class.
	 * This stores the entities in a sparse array which can be accessed using the index that they're associated with.
	 */
	template<class Type>
	class EntityContainer final : public IEntityContainer
	{
	public:
		/**
		 * Default constructor.
		 */
		EntityContainer() = default;

		/**
		 * Create a new entity.
		 *
		 * @tparam Arguments The argument types.
		 * @param arguments The arguments.
		 * @return The index and the created entity pointer.
		 */
		template<class...Arguments>
		[[nodiscard]] decltype(auto) create(Arguments&&... arguments)
		{
			return m_Container.insert(std::forward<Arguments>(arguments)...);
		}

	private:
		SparseArray<Type> m_Container;
	};

	/**
	 * Entity storage class.
	 * This class can store multiple different types of entities (it can store almost anything, really).
	 *
	 * The storage stores these entities statically, meaning that almost anyone can access these entities from anywhere in the
	 * application. But of course, only the application knows what these entities are so they formally belong to the application.
	 */
	class EntityStorage final
	{
	public:
		/**
		 * Default constructor.
		 */
		EntityStorage() = default;

		/**
		 * Check if an entity type is registered in the storage.
		 *
		 * @tparam Type The entity type.
		 * @return True if the entity type is stored.
		 * @return False if an entity type is not stored.
		 */
		template<class Type>
		[[nodiscard]] bool isRegistered() const { return m_pEntityContainers.contains(GetTypeIndex<Type>()); }

		/**
		 * Register an entity type in the system.
		 * This will not register the type if it's registered already.
		 *
		 * @tparam Type The entity type.
		 */
		template<class Type>
		void registerEntityType()
		{
			// Return if the type is already registered.
			if (isRegistered<Type>())
				return;

			// Else create the entity container.
			m_pEntityContainers[GetTypeIndex<Type>()] = std::make_shared<EntityContainer<Type>>();
		}

		/**
		 * Get an entity container from the storage.
		 * This will register the entity if it's not registered.
		 *
		 * @tparam Type The entity type.
		 * @return The entity container reference.
		 */
		template<class Type>
		[[nodiscard]] EntityContainer<Type>& getContainer()
		{
			// Register the entity type.
			registerEntityType<Type>();

			// Cast the entity pointer and dereference it.
			return *static_cast<EntityContainer<Type>*>(m_pEntityContainers[GetTypeIndex<Type>()].get());
		}

		/**
		 * Create a new entity.
		 *
		 * @tparam Type The entity type.
		 * @tparam Arguments The argument types.
		 * @param arguments The arguments.
		 * @return The index and the created entity pointer.
		 */
		template<class Type, class...Arguments>
		[[nodiscard]] decltype(auto) create(Arguments&&... arguments)
		{
			return getContainer<Type>().create(std::forward<Arguments>(arguments)...);
		}

	private:
		std::unordered_map<std::type_index, std::shared_ptr<IEntityContainer>> m_pEntityContainers;
	};
}