// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DX12Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Direct X 12 device bound object.
		 * This is the base class for all the Direct X 12 backend objects which is bound to the Direct X 12 device.
		 */
		class DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit DX12DeviceBoundObject(DX12Device* pDevice) : m_pDevice(pDevice) {}

			/**
			 * Move constructor.
			 *
			 * @param other The other device bound object.
			 */
			DX12DeviceBoundObject(DX12DeviceBoundObject&& other) noexcept : m_pDevice(std::exchange(other.m_pDevice, nullptr)) {}

			// Disable copy for the class.
			XENON_DISABLE_COPY(DX12DeviceBoundObject);

			/**
			 * Default virtual destructor.
			 */
			virtual ~DX12DeviceBoundObject() = default;

			/**
			 * Get the Direct X 12 device pointer.
			 *
			 * @return The device pointer.
			 */
			[[nodiscard]] DX12Device* getDevice() { return m_pDevice; }

			/**
			 * Get the Direct X 12 device pointer.
			 *
			 * @return The const device pointer.
			 */
			[[nodiscard]] const DX12Device* getDevice() const { return m_pDevice; }

		public:
			/**
			 * Move assignment operator.
			 *
			 * @param other The other device bound object.
			 * @return The moved device bound object.
			 */
			DX12DeviceBoundObject& operator=(DX12DeviceBoundObject&& other) noexcept { m_pDevice = std::exchange(other.m_pDevice, nullptr); return *this; }

		protected:
			DX12Device* m_pDevice = nullptr;
		};
	}
}