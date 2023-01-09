// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Scene.hpp"

namespace Xenon
{
	Scene::Scene(Instance& instance, std::unique_ptr<Backend::Camera>&& pCamera)
		: m_Instance(instance)
		, m_pCamera(std::move(pCamera))
	{
	}

	std::future<void> Scene::createMeshStorage(Group group, const std::filesystem::path& file)
	{
		return GetJobSystem().insert([this, group, file] { [[maybe_unused]] const auto& result = create<Geometry>(group, Geometry::FromFile(m_Instance, file)); });
	}

	void Scene::update()
	{

	}
}