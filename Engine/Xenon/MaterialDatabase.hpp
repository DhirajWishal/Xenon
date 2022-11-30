// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "MaterialBlob.hpp"

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
		 * Create a new material.
		 *
		 * @tparam Material The material type.
		 * @tparam Arguments The constructor argument types.
		 * @param identifier The material identifier.
		 * @param arguments The constructor arguments.
		 * @return The created material pointer.
		 */
		template<class Material, class...Arguments>
		Material* create(const std::string& identifier, Arguments&&... arguments)
		{
			auto pMaterial = std::make_unique<Material>(std::forward<Arguments>(arguments)...);
			auto pRawPointer = material.get();
			m_pMaterialBlobs[identifier] = std::move(pMaterial);

			return pRawPointer;
		}

		/**
		 * Get the material blob using the identifier.
		 *
		 * @param identifier The material's identifier.
		 * @return The material blob.
		 */
		[[nodiscard]] MaterialBlob* get(const std::string& identifier) { return m_pMaterialBlobs[identifier].get(); }

		/**
		 * Get the material blob using the identifier.
		 * Using this function the user can cast to whatever the type they want.
		 *
		 * @tparam Material The material type.
		 * @param identifier The material's identifier.
		 * @return The casted material pointer.
		 */
		template<class Material>
		[[nodiscard]] Material* getAs(const std::string& identifier) { return static_cast<Material*>(get(identifier)); }

		/**
		 * Check if the database contains a material.
		 *
		 * @param identifier The identifier to check.
		 * @return True if the material is present.
		 * @return False if the material is not present.
		 */
		[[nodiscard]] bool contains(const std::string& identifier) const { return m_pMaterialBlobs.contains(identifier); }

		/**
		 * Clear the material database.
		 */
		void clear() noexcept { m_pMaterialBlobs.clear(); }

	private:
		std::unordered_map<std::string, std::unique_ptr<MaterialBlob>> m_pMaterialBlobs;
	};
}