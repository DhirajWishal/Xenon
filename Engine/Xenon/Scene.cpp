// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Scene.hpp"

namespace Xenon
{
	Scene::Scene(Instance& instance, std::unique_ptr<Backend::Camera>&& pCamera)
		: m_Instance(instance)
		, m_pCamera(std::move(pCamera))
	{
		m_Registry.on_construct<Geometry>().connect<&Scene::onGeometryConstruction>(this);
		m_Registry.on_construct<Material>().connect<&Scene::onMaterialConstruction>(this);
	}

	std::future<void> Scene::createMeshStorage(Group group, const std::filesystem::path& file)
	{
		return GetJobSystem().insert([this, group, file] { [[maybe_unused]] const auto& result = create<Geometry>(group, Geometry::FromFile(m_Instance, file)); });
	}

	void Scene::update()
	{
		m_pCamera->update();
	}

	void Scene::onGeometryConstruction(entt::registry& registry, Group group)
	{
		if (registry.any_of<Material>(group))
		{
			for (const auto& geometry = registry.get<Geometry>(group); const auto & mesh : geometry.getMeshes())
				m_DrawableCount += mesh.m_SubMeshes.size();
		}
	}

	void Scene::onMaterialConstruction(entt::registry& registry, Group group)
	{
		if (registry.any_of<Geometry>(group))
		{
			for (const auto& geometry = registry.get<Geometry>(group); const auto & mesh : geometry.getMeshes())
				m_DrawableCount += mesh.m_SubMeshes.size();
		}
	}
}