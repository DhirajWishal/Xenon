// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Scene.hpp"

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Xenon
{
	Scene::Scene(Instance& instance, std::unique_ptr<Backend::Camera>&& pCamera)
		: m_Instance(instance)
		, m_pCamera(std::move(pCamera))
	{
		// Setup connections.
		m_Registry.on_construct<Geometry>().connect<&Scene::onGeometryConstruction>(this);
		m_Registry.on_construct<Material>().connect<&Scene::onMaterialConstruction>(this);

		m_Registry.on_construct<Components::Transform>().connect<&Scene::onTransformComponentConstruction>(this);
		m_Registry.on_update<Components::Transform>().connect<&Scene::onTransformComponentUpdate>(this);
		m_Registry.on_destroy<Components::Transform>().connect<&Scene::onTransformComponentDestruction>(this);

		// Setup the buffers.
		m_pSceneInformationUniform = m_Instance.getFactory()->createBuffer(m_Instance.getBackendDevice(), sizeof(SceneInformation), Backend::BufferType::Uniform);
		m_pLightSourceUniform = m_Instance.getFactory()->createBuffer(m_Instance.getBackendDevice(), sizeof(Components::LightSource), Backend::BufferType::Uniform);
	}

	void Scene::update()
	{
		const auto lock = std::scoped_lock(m_Mutex);

		setupLights();

		m_pSceneInformationUniform->write(ToBytes(&m_SceneInformation), sizeof(SceneInformation));
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
			case Xenon::Backend::SceneBindings::SceneInformation:
				pSceneDescriptor->attach(resource.m_Binding, m_pSceneInformationUniform.get());
				break;

			case Xenon::Backend::SceneBindings::Camera:
				pSceneDescriptor->attach(resource.m_Binding, m_pCamera->getViewports().front().m_pUniformBuffer);
				break;

			case Xenon::Backend::SceneBindings::LightSources:
				pSceneDescriptor->attach(resource.m_Binding, m_pLightSourceUniform.get());
				break;

			case Xenon::Backend::SceneBindings::AccelerationStructure:
				break;

			case Xenon::Backend::SceneBindings::RenderTarget:
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

			m_DrawableGeometryCount++;
		}
	}

	void Scene::onMaterialConstruction(entt::registry& registry, Group group)
	{
		if (registry.any_of<Geometry>(group))
		{
			for (const auto& geometry = registry.get<Geometry>(group); const auto & mesh : geometry.getMeshes())
				m_DrawableCount += mesh.m_SubMeshes.size();

			m_DrawableGeometryCount++;
		}
	}

	void Scene::onTransformComponentConstruction(entt::registry& registry, Group group)
	{
		const auto& transform = registry.get<Components::Transform>(group);
		const auto modelMatrix = transform.computeModelMatrix();

		auto& uniformBuffer = registry.emplace<Internal::TransformUniformBuffer>(group, m_Instance.getFactory()->createBuffer(m_Instance.getBackendDevice(), sizeof(modelMatrix), Backend::BufferType::Uniform));
		uniformBuffer.m_pUniformBuffer->write(ToBytes(glm::value_ptr(modelMatrix)), sizeof(modelMatrix));
	}

	void Scene::onTransformComponentUpdate(entt::registry& registry, Group group) const
	{
		const auto& transform = registry.get<Components::Transform>(group);
		const auto modelMatrix = transform.computeModelMatrix();

		registry.get<Internal::TransformUniformBuffer>(group).m_pUniformBuffer->write(ToBytes(glm::value_ptr(modelMatrix)), sizeof(modelMatrix));

	}

	void Scene::onTransformComponentDestruction(entt::registry& registry, Group group) const
	{
		registry.remove<Internal::TransformUniformBuffer>(group);
	}

	void Scene::setupLights()
	{
		std::vector<Components::LightSource> lightSources;
		for (const auto group : m_Registry.view<Components::LightSource>())
			lightSources.emplace_back(m_Registry.get<Components::LightSource>(group));

		const auto requiredSize = lightSources.size() * sizeof(Components::LightSource);
		const auto currentSize = m_pLightSourceUniform->getSize();

		if (requiredSize > currentSize)
			m_pLightSourceUniform = m_Instance.getFactory()->createBuffer(m_Instance.getBackendDevice(), requiredSize, Backend::BufferType::Uniform);

		m_pLightSourceUniform->write(ToBytes(lightSources.data()), requiredSize);
	}
}