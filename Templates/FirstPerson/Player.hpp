// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GameFramework/Entity.hpp"

/**
 * Player class.
 * This is the root entity of the first person game and will control the camera and everything else.
 */
class Player final : public Xenon::Entity
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param pParent The parent entity pointer.
	 */
	explicit Player(Xenon::Entity* pParent);

	/**
	 * On update method override.
	 *
	 * @param delta The delta time.
	 */
	void onUpdate(Xenon::DeltaTime delta) override;
};