// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Entity.hpp"

#include <tuple>

namespace Xenon
{
	/**
	 * Universal entity class.
	 * This class does not have any behaviors, it just holds a set of unique components. This class is a quick and easy way to instantiate
	 * an entity to the scene.
	 *
	 * @tparam Components The components held by the entity.
	 */
	template<class... Components>
	class UniversalEntity final : public Entity
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @tparam Arguments The argument types.
		 * @param pParent The parent entity pointer.
		 * @param arguments The arguments to initialize the components with.
		 */
		template<class... Arguments>
		explicit UniversalEntity(Entity* pParent, Arguments&&... arguments) : Entity(pParent), m_Components(std::forward<Arguments>(arguments)...) {}

		/**
		 * On update function override.
		 * Since the update function does not have any other functionalities, it will only update the components.
		 *
		 * @param delta The delta time taken by the frame. This will not be used by the function.
		 */
		void onUpdate([[maybe_unused]] DeltaTime delta) override { std::apply([this](auto&...components) { updateComponents(components...); }, m_Components); }

	public:
		/**
		 * Get the component of the type from the internal storage.
		 *
		 * @tparam Component The component type.
		 * @return The component reference.
		 */
		template<class Component>
		[[nodiscard]] Component& getComponent() { return std::get<Component>(m_Components); }

		/**
		 * Get the component of the type from the internal storage.
		 *
		 * @tparam Component The component type.
		 * @return The const component reference.
		 */
		template<class Component>
		[[nodiscard]] const Component& getComponent() const { return std::get<Component>(m_Components); }

	private:
		std::tuple<Components...> m_Components;
	};
}