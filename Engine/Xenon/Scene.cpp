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

	void Scene::update()
	{

	}
}