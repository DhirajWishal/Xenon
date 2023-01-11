// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Shader.hpp"
#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		class Descriptor;

		/**
		 * Descriptor builder class.
		 */
		class DescriptorBuilder final
		{
		public:
			/**
			 * Default constructor.
			 */
			DescriptorBuilder() = default;

			/**
			 * Add a binding to the binding info.
			 * Make sure that bindings are added in the order they should appear in shaders.
			 *
			 * @param applicableShaders The shaders to which the resource will be passed to.
			 * @param resourceType The resource type.
			 * @return The builder reference used to chain.
			 */
			DescriptorBuilder& addBinding(ShaderType applicableShaders, ResourceType resourceType) { m_BindingInfo.emplace_back(applicableShaders, resourceType); return *this; }

			/**
			 * Get the binding info.
			 *
			 * @return The binding info.
			 */
			[[nodiscard]] const std::vector<DescriptorBindingInfo>& getBindingInfo() const noexcept { return m_BindingInfo; }

		private:
			std::vector<DescriptorBindingInfo> m_BindingInfo = {};
		};

		/**
		 * Descriptor manager class.
		 * This class is used to create and manage a single type of descriptors.
		 */
		class DescriptorManager : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit DescriptorManager([[maybe_unused]] const Device* pDevice) {}

			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param type The descriptor type.
			 * @param bindingInfo The descriptor's binding info.
			 */
			explicit DescriptorManager([[maybe_unused]] const Device* pDevice, DescriptorType type, const std::vector<DescriptorBindingInfo>& bindingInfo) : m_BindingInfo(bindingInfo), m_Type(type) {}

			/**
			 * Create a new descriptor.
			 *
			 * @return The created descriptor.
			 */
			[[nodiscard]] virtual std::unique_ptr<Descriptor> create() = 0;

		protected:
			std::vector<DescriptorBindingInfo> m_BindingInfo = {};
			DescriptorType m_Type = DescriptorType::UserDefined;
		};
	}
}