// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Xenon
{
	/**
	 * Entity class.
	 * This is the base class for all the entities in a game.
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

	private:
		Entity* m_pParent = nullptr;
	};
}