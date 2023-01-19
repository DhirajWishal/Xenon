// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Instance.hpp"

#include "DX12Includes.hpp"

using Microsoft::WRL::ComPtr;

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 instance class.
		 */
		class DX12Instance final : public Instance
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param appliationName The name of the application.
			 * @param applicationVersion The application version.
			 */
			explicit DX12Instance(const std::string& applicationName, uint32_t applicationVersion);

			/**
			 * Destructor.
			 */
			~DX12Instance() override = default;

		public:
			/**
			 * Get the DXGI factory flags.
			 *
			 * @return The flags.
			 */
			[[nodiscard]] UINT getFactoryFlags() const { return m_FactoryFlags; }

		private:
			ComPtr<ID3D12Debug> m_Debugger;
			UINT m_FactoryFlags = 0;
		};
	}
}