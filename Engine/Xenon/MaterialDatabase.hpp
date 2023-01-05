// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "MaterialBlob.hpp"

#include <list>

namespace Xenon
{
	/**
	 * Material database class.
	 * This class contains information about materials in a single engine instance.
	 */
	class MaterialDatabase final : public XObject
	{
		/**
		 * Interface database entry structure.
		 */
		struct IDatabaseEntry
		{
			IDatabaseEntry() = default;
			virtual ~IDatabaseEntry() = default;
		};

		/**
		 * Database entry structure.
		 * This structure contains multiple materials with unique hashes used to identify the material instances.
		 */
		template<class Material>
		struct DatabaseEntry final : public IDatabaseEntry
		{
			std::unordered_map<uint64_t, std::unique_ptr<Material>> m_pMaterials;
		};

	public:
		/**
		 * Default constructor.
		 */
		MaterialDatabase() = default;

		/**
		 * Get a material entry from the database.
		 *
		 * @tparam Material The material type.
		 * @return The database entry pointer.
		 */
		template<class Material>
		[[nodiscard]] DatabaseEntry<Material>* getMaterialEntry()
		{
			if (!isRegistered<Material>())
				registerMaterial<Material>();

			return static_cast<DatabaseEntry<Material>*>(m_pDatabaseEntries[getTypeIndex<Material>()].get());
		}

		/**
		 * Get a material entry from the database.
		 *
		 * @tparam Material The material type.
		 * @return The database entry pointer.
		 */
		template<class Material>
		[[nodiscard]] const DatabaseEntry<Material>* getMaterialEntry() const
		{
			if (!isRegistered<Material>())
				return nullptr;

			return static_cast<const DatabaseEntry<Material>*>(m_pDatabaseEntries.at(getTypeIndex<Material>()).get());
		}

		/**
		 * Create a new material.
		 *
		 * @tparam Material The material type.
		 * @tparam Arguments The constructor argument types.
		 * @param hash The material hash.
		 * @param arguments The constructor arguments.
		 * @return The created material's identifier.
		 */
		template<class Material, class...Arguments>
		[[nodiscard]] MaterialIdentifier create(uint64_t hash, Arguments&&... arguments)
		{
			MaterialIdentifier identifier;
			identifier.m_pMaterial = getMaterialEntry<Material>()->m_pMaterials.emplace(hash, std::make_unique<Material>(std::forward<Arguments>(arguments)...)).first->second.get();
			identifier.m_MaterialTypeIndex = getTypeIndex<Material>();

			return identifier;
		}

		/**
		 * Get a material from the system.
		 *
		 * @tparam Material The material type.
		 * @param hash The material hash.
		 * @return The material's identifier.
		 */
		template<class Material>
		[[nodiscard]] MaterialIdentifier get(uint64_t hash)
		{
			MaterialIdentifier identifier;
			identifier.m_pMaterial = getMaterialEntry<Material>()->m_pMaterials[hash].get();
			identifier.m_MaterialTypeIndex = getTypeIndex<Material>();

			return identifier;
		}

		/**
		 * Check if a material is stored in the database.
		 *
		 * @tparam Material The material type.
		 * @param hash The material hash.
		 * @return True if the material is stored in the system.
		 * @return False if the material is not stored in the system.
		 */
		template<class Material>
		[[nodiscard]] bool contains(uint64_t hash) const { return isRegistered<Material>() && getMaterialEntry<Material>()->m_pMaterials.contains(hash); }

		/**
		 * Clear the material database.
		 */
		void clear() noexcept { m_pDatabaseEntries.clear(); }

	private:
		/**
		 * Get the type index of a material.
		 *
		 * @tparam Material The material type.
		 * @return The material type's type index.
		 */
		template<class Material>
		[[nodiscard]] std::type_index getTypeIndex() const { return std::type_index(typeid(Material)); }

		/**
		 * Check if a material is registered in the system.
		 *
		 * @tparam Material The material type to check.
		 * @return True if the material is registered.
		 * @return False if the material is not registered.
		 */
		template<class Material>
		[[nodiscard]] bool isRegistered() const { return m_pDatabaseEntries.contains(getTypeIndex<Material>()); }

		/**
		 * Register a new material type.
		 *
		 * @tparam Material The material type to register.
		 */
		template<class Material>
		void registerMaterial() { m_pDatabaseEntries[getTypeIndex<Material>()] = std::make_unique<DatabaseEntry<Material>>(); }

	private:
		std::unordered_map<std::type_index, std::unique_ptr<IDatabaseEntry>> m_pDatabaseEntries;
	};
}