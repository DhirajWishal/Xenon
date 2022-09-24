// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "FirstPerson.hpp"
#include "Player.hpp"

#include "Common/Logging.hpp"

FirstPerson::FirstPerson()
	: Application("First Person", 1)
{
	XENON_LOG_INFORMATION("Hello from the first person application!");

	const auto player = std::make_unique<Player>(nullptr);
}

FirstPerson::~FirstPerson()
{

}

void FirstPerson::onUpdate(std::chrono::microseconds delta)
{

}

XENON_SETUP_APPLICATION(FirstPerson)