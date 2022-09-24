// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Xenon.hpp"

/**
 * First person class.
 */
class FirstPerson final : public Xenon::Application
{
public:
	/**
	 * Constructor.
	 */
	FirstPerson();

	/**
	 * Destructor.
	 */
	~FirstPerson();

	/**
	 * On update method override.
	 *
	 * @param delta The delta time difference.
	 */
	void onUpdate(std::chrono::microseconds delta) override;
};