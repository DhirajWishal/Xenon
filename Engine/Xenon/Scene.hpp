// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "Components.hpp"
#include "Geometry.hpp"

#include "../XenonBackend/Camera.hpp"

#include <entt/entt.hpp>

namespace Xenon
{
	/**
	 * Group type.
	 * This is just an entt::entity type reference and is used to group objects together.
	 */
	using Group = entt::entity;

	/**
	 * Scene information structure.
	 * This is a shader-visible structure which contains the information about a single scene.
	 */
	struct SceneInformation final
	{
		XENON_HLSL_VEC3_ALIGNMENT uint32_t m_LightSourceCount = 0;
	};

	namespace Internal
	{
		/**
		 * Transform uniform buffer structure.
		 * This is intended to store a single transform component's data.
		 */
		struct TransformUniformBuffer final
		{
			std::unique_ptr<Backend::Buffer> m_pUniformBuffer = nullptr;
		};
	}

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
		 * Begin updating the scene.
		 * This must be done to create new groups, objects and others.
		 * Call this function at the beginning of a new frame.
		 */
		void beginUpdate();

		/**
		 * End updating the scene.
		 * This must be done to update the internal buffers.
		 * Call this function right before updating the renderer.
		 */
		void endUpdate();

		/**
		 * Cleanup everything.
		 * Make sure the call this method before anything else right after terminating the main loop to not cause any deadlocks.
		 */
		void cleanup();

		/**
		 * Create a new group.
		 *
		 * @return The created group.
		 */
		XENON_NODISCARD Group createGroup() { const auto lock = std::scoped_lock(m_Mutex); return m_Registry.create(); }

		/**
		 * Create a new object.
		 *
		 * @tparam Object The object type.
		 * @tparam Arguments The argument types.
		 * @param group The object's grouping.
		 * @param arguments The constructor arguments.
		 * @return The created object reference.
		 */
		template<class Object, class... Arguments>
		XENON_NODISCARD Object& create(Group group, Arguments&&... arguments)
		{
			const auto lock = std::scoped_lock(m_Mutex);
			return m_Registry.emplace<Object>(group, std::forward<Arguments>(arguments)...);
		}

		/**
		 * Create a new material object.
		 *
		 * @param group The object's grouping.
		 * @param builder The material builder class.
		 * @return The created material reference.
		 */
		XENON_NODISCARD Material& createMaterial(Group group, MaterialBuilder& builder);

		/**
		 * Get a stored object from the registry.
		 *
		 * @tparam Object The object type.
		 * @param group The group of the object.
		 * @return The stored object reference.
		 */
		template<class Object>
		XENON_NODISCARD Object& get(Group group)
		{
			const auto lock = std::scoped_lock(m_Mutex);
			return m_Registry.get<Object>(group);
		}

		/**
		 * Get a stored object from the registry.
		 *
		 * @tparam Object The object type.
		 * @param group The group of the object.
		 * @return The stored object reference.
		 */
		template<class Object>
		XENON_NODISCARD const Object& get(Group group) const
		{
			return m_Registry.get<Object>(group);
		}

		/**
		 * Setup the scene descriptor for a given pipeline.
		 *
		 * @param pSceneDescriptor The scene descriptor pointer.
		 * @param pPipeline The pipeline pointer.
		 */
		void setupDescriptor(Backend::Descriptor* pSceneDescriptor, const Backend::RasterizingPipeline* pPipeline);

		/**
		 * Get the object registry.
		 *
		 * @return The registry reference.
		 */
		XENON_NODISCARD entt::registry& getRegistry() noexcept { return m_Registry; }

		/**
		 * Get the object registry.
		 *
		 * @return The registry reference.
		 */
		XENON_NODISCARD const entt::registry& getRegistry() const noexcept { return m_Registry; }

		/**
		 * Get the instance.
		 *
		 * @return The instance reference.
		 */
		XENON_NODISCARD Instance& getInstance() noexcept { return m_Instance; }

		/**
		 * Get the instance.
		 *
		 * @return The instance reference.
		 */
		XENON_NODISCARD const Instance& getInstance() const noexcept { return m_Instance; }

		/**
		 * Get the camera pointer.
		 *
		 * @return The pointer.
		 */
		XENON_NODISCARD Backend::Camera* getCamera() noexcept { return m_pCamera.get(); }

		/**
		 * Get the camera pointer.
		 *
		 * @return The pointer.
		 */
		XENON_NODISCARD const Backend::Camera* getCamera() const noexcept { return m_pCamera.get(); }

		/**
		 * Get the drawable count.
		 * This is the number of objects that can be drawn by a layer (geometry + material).
		 *
		 * @return The total sub-mesh count.
		 */
		XENON_NODISCARD uint64_t getDrawableCount() const noexcept { return m_DrawableCount; }

		/**
		 * Get the drawable geometry count.
		 *
		 * @return The geometry count.
		 */
		XENON_NODISCARD uint64_t getDrawableGeometryCount() const noexcept { return m_DrawableGeometryCount; }

		/**
		 * Get the scene object's mutex.
		 * This might be needed when synchronizing draw calls.
		 *
		 * @return The mutex reference.
		 */
		XENON_NODISCARD std::mutex& getMutex() noexcept { return m_Mutex; }

	private:
		/**
		 * On geometry construction callback.
		 * This is called by the ECS registry when a new geometry is added.
		 *
		 * @param registry The registry to which the geometry is added. In our case it's the same as m_Registry.
		 * @param group The group to which the geometry is added.
		 */
		void onGeometryConstruction(entt::registry& registry, Group group);

		/**
		 * On material construction callback.
		 * This is called by the ECS registry when a new material is added.
		 *
		 * @param registry The registry to which the material is added. In our case it's the same as m_Registry.
		 * @param group The group to which the material is added.
		 */
		void onMaterialConstruction(entt::registry& registry, Group group);

		/**
		 * On transform component construction callback.
		 * This is called by the ECS registry when a new transform component is added.
		 *
		 * @param registry The registry to which the transform component is added. In our case it's the same as m_Registry.
		 * @param group The group to which the transform component is added.
		 */
		void onTransformComponentConstruction(entt::registry& registry, Group group);

		/**
		 * On transform component update callback.
		 * This is called by the ECS registry when a new transform component is updated.
		 *
		 * @param registry The registry in which the component was updated. In our case it's the same as m_Registry.
		 * @param group The group to which the transform component is updated.
		 */
		void onTransformComponentUpdate(entt::registry& registry, Group group) const;

		/**
		 * On transform component destruction callback.
		 * This is called by the ECS registry when a new transform component is removed.
		 *
		 * @param registry The registry to which the transform component is removed. In our case it's the same as m_Registry.
		 * @param group The group to which the transform component is removed.
		 */
		void onTransformComponentDestruction(entt::registry& registry, Group group) const;

		/**
		 * Setup the lighting.
		 */
		void setupLights();

	private:
		entt::registry m_Registry;
		std::mutex m_Mutex;
		std::unique_lock<std::mutex> m_UniqueLock;

		Instance& m_Instance;

		SceneInformation m_SceneInformation = {};

		std::unique_ptr<Backend::Camera> m_pCamera = nullptr;

		std::unique_ptr<Backend::Buffer> m_pSceneInformationUniform = nullptr;
		std::unique_ptr<Backend::Buffer> m_pLightSourceUniform = nullptr;

		uint64_t m_DrawableCount = 0;
		uint64_t m_DrawableGeometryCount = 0;

		std::atomic_bool m_IsUpdatable = true;
	};
}