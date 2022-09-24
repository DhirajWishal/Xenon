// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Player.hpp"

#include "GameFramework/UniversalEntity.hpp"

Player::Player(Xenon::Entity* pParent)
	: Entity(pParent)
{
	auto entry = spawn<Xenon::UniversalEntity<int>>();
}

void Player::onUpdate(Xenon::DeltaTime delta)
{

}
