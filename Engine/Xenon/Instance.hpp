// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/IFactory.hpp"
#include "MaterialDatabase.hpp"

#include <string>

namespace Xenon
{
	/**
	 * Backend type enum.
	 */
	enum class BackendType : uint8_t
	{
		Vulkan,
		DirectX_12,

		Any
	};

	/**
	 * Instance class.
	 * This is the main class which the user needs to instantiate to use the engine.
	 *
	 * If the requested render target types are not available by the device, it will only enable the render targets which are supported.
	 * A warning will be issued for this issue.
	 */
	class Instance final : public XObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param applicationName The name of the application.
		 * @param applicationVersion The version of the application.
		 * @param renderTargets The render targets which the application will use.
		 * @param backendType The backend type to use. Default is any.
		 */
		explicit Instance(const std::string& applicationName, uint32_t applicationVersion, RenderTargetType renderTargets, BackendType backendType = BackendType::Any);

		/**
		 * Destructor.
		 */
		~Instance() override;

		/**
		 * Cleanup the internal commands and finish their execution.
		 */
		void cleanup() const;

	public:
		/**
		 * Get the application name.
		 *
		 * @return The application name string view.
		 */
		[[nodiscard]] const std::string_view getApplicationName() const noexcept { return m_ApplicationName; }

		/**
		 * Get the application version.
		 *
		 * @return The application version.
		 */
		[[nodiscard]] uint32_t getApplicationVersion() const noexcept { return m_ApplicationVersion; }

		/**
		 * Get the supported render target types.
		 *
		 * @return The render target types.
		 */
		[[nodsicard]] RenderTargetType getSupportedRenderTargetTypes() const { return m_pDevice->getSupportedRenderTargetTypes(); }

		/**
		 * Get the current backend type that's been used.
		 *
		 * @return The backend type.
		 */
		[[nodiscard]] BackendType getBackendType() const noexcept { return m_BackendType; }

		/**
		 * Get the backend factory.
		 *
		 * @return The backend factory pointer.
		 */
		[[nodiscard]] Backend::IFactory* getFactory() noexcept { return m_pFactory.get(); }

		/**
		 * Get the backend factory.
		 *
		 * @return The const backend factory pointer.
		 */
		[[nodiscard]] const Backend::IFactory* getFactory() const noexcept { return m_pFactory.get(); }

		/**
		 * Get the backend instance pointer.
		 *
		 * @return The instance pointer.
		 */
		[[nodiscard]] Backend::Instance* getBackendInstance() noexcept { return m_pInstance.get(); }

		/**
		 * Get the backend instance pointer.
		 *
		 * @return The const instance pointer.
		 */
		[[nodiscard]] const Backend::Instance* getBackendInstance() const noexcept { return m_pInstance.get(); }

		/**
		 * Get the backend device pointer.
		 *
		 * @return The device pointer.
		 */
		[[nodiscard]] Backend::Device* getBackendDevice() noexcept { return m_pDevice.get(); }

		/**
		 * Get the backend device pointer.
		 *
		 * @return The const device pointer.
		 */
		[[nodiscard]] const Backend::Device* getBackendDevice() const noexcept { return m_pDevice.get(); }

		/**
		 * Get the default image pointer.
		 *
		 * @return The image pointer.
		 */
		[[nodiscard]] Backend::Image* getDefaultImage() noexcept { return m_pDefaultImage.get(); }

		/**
		 * Get the default image pointer.
		 *
		 * @return The image pointer.
		 */
		[[nodiscard]] const Backend::Image* getDefaultImage() const noexcept { return m_pDefaultImage.get(); }

		/**
		 * Get the default image view pointer.
		 *
		 * @return The image view pointer.
		 */
		[[nodiscard]] Backend::ImageView* getDefaultImageView() noexcept { return m_pDefaultImageView.get(); }

		/**
		 * Get the default image view pointer.
		 *
		 * @return The image view pointer.
		 */
		[[nodiscard]] const Backend::ImageView* getDefaultImageView() const noexcept { return m_pDefaultImageView.get(); }

		/**
		 * Get the default image sampler pointer.
		 *
		 * @return The image sampler pointer.
		 */
		[[nodiscard]] Backend::ImageSampler* getDefaultImageSampler() noexcept { return m_pDefaultImageSampler.get(); }

		/**
		 * Get the default image sampler pointer.
		 *
		 * @return The image sampler pointer.
		 */
		[[nodiscard]] const Backend::ImageSampler* getDefaultImageSampler() const noexcept { return m_pDefaultImageSampler.get(); }

		/**
		 * Get the material database.
		 *
		 * @return The database reference.
		 */
		[[nodiscard]] MaterialDatabase& getMaterialDatabase() { return m_MaterialDatabase; }

		/**
		 * Get the material database.
		 *
		 * @return The const database reference.
		 */
		[[nodiscard]] const MaterialDatabase& getMaterialDatabase() const { return m_MaterialDatabase; }

	private:
		std::string m_ApplicationName;
		uint32_t m_ApplicationVersion;

		std::unique_ptr<Backend::IFactory> m_pFactory = nullptr;
		std::unique_ptr<Backend::Instance> m_pInstance = nullptr;
		std::unique_ptr<Backend::Device> m_pDevice = nullptr;

		std::unique_ptr<Backend::Image> m_pDefaultImage = nullptr;
		std::unique_ptr<Backend::ImageView> m_pDefaultImageView = nullptr;
		std::unique_ptr<Backend::ImageSampler> m_pDefaultImageSampler = nullptr;

		MaterialDatabase m_MaterialDatabase;

		BackendType m_BackendType = BackendType::Any;
	};
}