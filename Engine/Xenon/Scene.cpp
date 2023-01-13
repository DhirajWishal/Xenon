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

	void Scene::setupDescriptor(Backend::Descriptor* pSceneDescriptor, Backend::RasterizingPipeline* pPipeline)
	{
		// Get all the unique resources.
		std::vector<Backend::ShaderResource> resources = pPipeline->getSpecification().m_VertexShader.getResources();
		for (const auto& resource : pPipeline->getSpecification().m_FragmentShader.getResources())
		{
			if (std::find(resources.begin(), resources.end(), resource) == resources.end())
				resources.emplace_back(resource);
		}

		// Setup the bindings.
		for (const auto& resource : resources)
		{
			// Continue if we have any other resource other than scene.
			if (resource.m_Set != Backend::DescriptorType::Scene)
				continue;

			// Bind everything that we need!
			switch (static_cast<Backend::SceneBindings>(resource.m_Binding))
			{
			case Xenon::Backend::SceneBindings::Camera:
				pSceneDescriptor->attach(resource.m_Binding, m_pCamera->getViewports().front().m_pUniformBuffer);
				break;

			default:
				break;
			}
		}
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