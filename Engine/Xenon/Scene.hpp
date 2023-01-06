// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "Components.hpp"

#include "../XenonBackend/Camera.hpp"

#include <entt/entt.hpp>

namespace Xenon
{
	using Group = entt::entity;

	/**
	 * Scene information structure.
	 * This is a shader-visible structure which contains the information about a single scene.
	 */
	struct SceneInformation final
	{
		uint32_t m_LightSourceCount = 0;
	};

	/**
	 * Scene class.
	 * This contains all the information related to a single scene.
	 */
	class Scene final : public XObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance reference.
		 * @param pCamera The camera pointer.
		 */
		explicit Scene(Instance& instance, std::unique_ptr<Backend::Camera>&& pCamera);

		/**
		 * Create a new component.
		 *
		 * @tparam Component The component type.
		 * @tparam Arguments The argument types.
		 * @param group The component's grouping.
		 * @param arguments The constructor arguments.
		 * @return The created component reference.
		 */
		template<class Component, class... Arguments>
		[[nodiscard]] Component& createComponent(Group group, Arguments&&... arguments)
		{
			return m_Registry.emplace<Component>(group, std::forward<Arguments>(arguments)...);
		}

		/**
		 * Update the internal buffers.
		 */
		void update();

		/**
		 * Get the instance.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] Instance& getInstance() noexcept { return m_Instance; }

		/**
		 * Get the instance.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] const Instance& getInstance() const noexcept { return m_Instance; }

		/**
		 * Get the camera pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] Backend::Camera* getCamera() noexcept { return m_pCamera.get(); }

		/**
		 * Get the camera pointer.
		 *
		 * @return The pointer.
		 */
		[[nodiscard]] const Backend::Camera* getCamera() const noexcept { return m_pCamera.get(); }

	private:
		entt::registry m_Registry;

		Instance& m_Instance;

		SceneInformation m_SceneInformation = {};

		std::unique_ptr<Backend::Camera> m_pCamera = nullptr;

		std::unique_ptr<Backend::Descriptor> m_pSceneDescriptor = nullptr;

		std::unique_ptr<Backend::Buffer> m_pLightSourceUniform = nullptr;
	};
}