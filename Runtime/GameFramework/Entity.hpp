// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Common/JobSystem.hpp"
#include "../Common/EntityStorage.hpp"

namespace Xenon
{
	/**
	 * Entity class.
	 * This is the base class for all the entities in a game.
	 *
	 * An entity can have multiple components. There are two main ways to store them, either using smart pointers or by instantiating them as a member variable.
	 * We encourage the use of Composition-Over-Inheritance. Do not take it as an advice to completely avoid inheritance altogether, use it where it fits.
	 * For components, it's the best to have them stored in the class as members than pointers (look into cache locality). Since components are not exposed to the
	 * engine directly, the entity must update them explicitly (using the updateComponent() or updateComponents() functions).
	 */
	class Entity
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pParent The parent entity pointer. If the pointer is nullptr, it might not get updated (unless it's a root/ parent entity).
		 */
		explicit Entity(Entity* pParent) : m_pParent(pParent) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~Entity() = default;

		/**
		 * Spawn a new entity.
		 * The new entity's parent will be the calling entity. Make sure that the entity type is derived from 'Entity'.
		 * The returned entity pointer might not be valid after spawning another entity.
		 *
		 * @tparam Type The entity type.
		 * @tparam Arguments The constructor argument types.
		 * @param arguments The constructor arguments.
		 * @return The pair containing the entity index and the pointer.
		 */
		template<class Type, class... Arguments>
		[[nodiscard]] decltype(auto) spawn(Arguments&&... arguments)
		{
			return getEntityStorage().create<Type>(std::forward<Arguments>(arguments)...);
		}

	public:
		/**
		 * Check if the entity has a parent.
		 *
		 * @return True if the object has a parent.
		 * @return False if the object doesn't have a parent.
		 */
		[[nodiscard]] bool hasParent() const { return m_pParent != nullptr; }

		/**
		 * Check if the entity is a parent.
		 * Entities are considered as parents if the parent pointer is a nullptr.
		 *
		 * @return True if the entity is a parent.
		 * @return False if the entity is a child entity.
		 */
		[[nodiscard]] bool isParent() const { return m_pParent == nullptr; }

		/**
		 * Get the parent pointer.
		 *
		 * @return The parent pointer.
		 */
		[[nodiscard]] Entity* getParent() { return m_pParent; }

		/**
		 * Get the parent pointer.
		 *
		 * @return The const parent pointer.
		 */
		[[nodiscard]] const Entity* getParent() const { return m_pParent; }

	public:
		/**
		 * Get the entity storage of the entity.
		 *
		 * @return The entity storage reference.
		 */
		[[nodiscard]] EntityStorage& getEntityStorage() { return m_EntityStorage; }

		/**
		 * Get the entity storage of the entity.
		 *
		 * @return The const entity storage reference.
		 */
		[[nodiscard]] const EntityStorage& getEntityStorage() const { return m_EntityStorage; }

		/**
		 * Store the entity index.
		 * The index can be retrieved by it's associated name.
		 * Note that we rather recommend storing this index as a member variable. This is just for convenience.
		 *
		 * @param index The index to store.
		 * @param name The associated name.
		 */
		void setEntityIndex(uint64_t index, const std::string& name) { m_EntityIndexMap[name] = index; }

		/**
		 * Get the entity index using it's associated name.
		 *
		 * @param name The name of the entity.
		 * @return The entity index.
		 */
		[[nodiscard]] uint64_t getEntityIndex(const std::string& name) { return m_EntityIndexMap[name]; }

	protected:
		/**
		 * Get the entity job system which executes all the necessary asynchronous tasks.
		 *
		 * @return The job system reference.
		 */
		[[nodiscard]] static JobSystem& getJobSystem();

	protected:
		/**
		 * Update multiple components.
		 *
		 * @tparam Components The component types.
		 * @param components The components to update.
		 */
		template<class... Components>
		void updateComponents(Components&... components)
		{
			// TODO: Implement the updating system.
		}

	private:
		EntityStorage m_EntityStorage;
		std::unordered_map<std::string, uint64_t> m_EntityIndexMap;
		Entity* m_pParent = nullptr;
	};
}