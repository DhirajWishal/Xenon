// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Material.hpp"

namespace Xenon
{
	/**
	 * Material database class.
	 * This class contains information about materials in a single engine instance.
	 */
	class MaterialDatabase final : public XObject
	{
	public:
		/**
		 * Default constructor.
		 */
		MaterialDatabase() = default;

		/**
		 * Store a material specification in the instance class.
		 * This enables us to store materials for a given database and reduce memory consumption.
		 *
		 * @param specification The material specification to store.
		 * @return The material.
		 */
		XENON_NODISCARD Material storeSpecification(const MaterialSpecification& specification)
		{
			const auto material = static_cast<Material>(GenerateHashFor(specification));
			if (!m_MaterialSpecifications.contains(material))
				m_MaterialSpecifications[material] = specification;

			return material;
		}

		/**
		 * Get the material specification using it's ID.
		 *
		 * @param material The material.
		 * @return The material specification reference.
		 */
		XENON_NODISCARD MaterialSpecification& getSpecification(Material material)
		{
			return m_MaterialSpecifications.at(material);
		}

		/**
		 * Get the material specification using it's ID.
		 *
		 * @param material The material.
		 * @return The material specification reference.
		 */
		XENON_NODISCARD const MaterialSpecification& getSpecification(Material material) const
		{
			return m_MaterialSpecifications.at(material);
		}

		/**
		 * Clear the material database.
		 */
		void clear() noexcept
		{
			m_MaterialSpecifications.clear();
		}

	private:
		std::unordered_map<Material, MaterialSpecification> m_MaterialSpecifications;
	};
}