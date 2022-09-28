// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "XenonCore/Logging.hpp"
#include "Xenon/Instance.hpp"

int main()
{
	XENON_LOG_INFORMATION("Hello from the Xenon Studio!");
	auto instance = Xenon::Instance("Xenon Studio", 0, Xenon::RenderTargetType::All);

	return 0;
}