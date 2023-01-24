// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "XenonCore/GlobalConfiguration.hpp"

#include "Xenon/Instance.hpp"

/**
 * Studio configuration class.
 * This contains all the studio-centric configuration.
 */
class StudioConfiguration final : public Xenon::GlobalConfiguration<StudioConfiguration>
{
public:
	/**
	 * Default constructor.
	 */
	StudioConfiguration() = default;

	/**
	 * Destructor.
	 */
	~StudioConfiguration() override = default;

	/**
	 * Get the current backend type.
	 * This will default to Any if none was defined.
	 *
	 * @return The backend type.
	 */
	[[nodiscard]] Xenon::BackendType getCurrentBackendType() const;

	/**
	 * Set the current backend type.
	 *
	 * @param type The backend type to set.
	 */
	void setCurrentBackendType(Xenon::BackendType type);

	/**
	 * Check if the application should exit.
	 *
	 * @return True if the application should exit.
	 * @return False if the application should not exit.
	 */
	[[nodiscard]] bool shouldExitApplication() const noexcept { return m_bExitAppliation; }

	/**
	 * Toggle the internal variable to true and indicate that the application should exit.
	 */
	void toggleExitApplication() noexcept { m_bExitAppliation = true; }

private:
	bool m_bExitAppliation = false;
};