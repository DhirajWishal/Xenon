// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GeometryStore.hpp"

namespace Xenon
{
	std::future<Geometry&> GeometryStore::create(const std::filesystem::path& path)
	{
		return GetJobSystem().insert([this, path]() -> Geometry& { return m_Geometries.emplace_back(Geometry::FromFile(m_Instance, path)); });
	}
}