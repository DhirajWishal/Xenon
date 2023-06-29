// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Image.hpp"

#include <variant>

namespace Xenon
{
	namespace Backend
	{
		class RayTracingPipeline;

		/**
		 * Binding group structure.
		 * This contains information about a single shader group's binding.
		 */
		struct BindingGroup final
		{
			using DataVariant = std::variant<Buffer*, Image*, std::pair<const std::byte*, uint64_t>>;

			/**
			 * Attach a buffer to the binding table.
			 *
			 * @param type The shader type.
			 * @param pBuffer The buffer pointer to bind.
			 */
			XENON_NODISCARD BindingGroup& attach(ShaderType type, Buffer* pBuffer) { m_Entries.emplace_back(type, pBuffer); return *this; }

			/**
			 * Attach a image to the binding table.
			 *
			 * @param type The shader type.
			 * @param pImage The image pointer to bind.
			 */
			XENON_NODISCARD BindingGroup& attach(ShaderType type, Image* pImage) { m_Entries.emplace_back(type, pImage); return *this; }

			/**
			 * Attach raw data to the binding table.
			 *
			 * @param type The shader type.
			 * @param pData The data pointer to bind.
			 * @param size The size of the data to bind.
			 */
			XENON_NODISCARD BindingGroup& attach(ShaderType type, const std::byte* pData, uint64_t size) { m_Entries.emplace_back(type, std::make_pair(pData, size)); return *this; }

			std::vector<std::pair<ShaderType, DataVariant>> m_Entries;
		};

		/**
		 * Shader binding table builder class.
		 * This class can be used to create shader binding tables for ray generation, miss, hit or callable shaders.
		 */
		class ShaderBindingTableBuilder final : public BackendObject
		{
		public:
			/**
			 * Default constructor.
			 */
			ShaderBindingTableBuilder() = default;

			/**
			 * Create a new group.
			 *
			 * @return The created group reference.
			 */
			XENON_NODISCARD BindingGroup& createGroup() { return m_BindingGroups.emplace_back(); }

			/**
			 * Get the stored binding groups.
			 *
			 * @return The binding groups.
			 */
			XENON_NODISCARD const std::vector<BindingGroup>& getBindingGroups() const noexcept { return m_BindingGroups; }

		private:
			std::vector<BindingGroup> m_BindingGroups;
		};

		/**
		 * Shader binding table class.
		 * This class is used to send data to shaders when performing ray tracing.
		 */
		class ShaderBindingTable : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pPipeline The ray tracing pipeline pointer.
			 * @param bindingGroups The binding groups.
			 */
			explicit ShaderBindingTable(XENON_MAYBE_UNUSED const Device* pDevice, XENON_MAYBE_UNUSED const RayTracingPipeline* pPipeline, const std::vector<BindingGroup>& bindingGroups) : m_BindingGroups(bindingGroups) {}

		protected:
			std::vector<BindingGroup> m_BindingGroups;

			uint64_t m_RayGenSize = 0;
			uint64_t m_RayMissSize = 0;
			uint64_t m_RayHitSize = 0;
			uint64_t m_CallableSize = 0;
		};
	}
}