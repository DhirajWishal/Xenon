// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GameFramework/Entity.hpp"

namespace Xenon
{
	/**
	 * Application class.
	 * This class is to be inherited by the game application.
	 *
	 * The constructor and destructor will initiate all the other sub systems of the application. These are the
	 * only two functions which will be called, so all the other functions are required to be executed from within
	 * these two functions, including the game loop.
	 */
	class Application
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param name The game name.
		 * @param version The game version.
		 */
		explicit Application(const std::string& name, uint64_t version);

		/**
		 * Virtual destructor.
		 */
		virtual ~Application() = default;

		/**
		 * Get the game name.
		 *
		 * @return The name.
		 */
		[[nodiscard]] std::string_view getName() const { return m_Name; }

		/**
		 * Get the game version.
		 *
		 * @return The version.
		 */
		[[nodiscard]] uint64_t getVersion() const { return m_Version; }

	private:
		const std::string m_Name;
		const uint64_t m_Version;
	};
}

/**
 * Xenon setup application macro.
 * This macro contains the entry point and other information required to setup the engine's entry point.
 */
#define XENON_SETUP_APPLICATION(object)		int main() { object instance; return 0; }