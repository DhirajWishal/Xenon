// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GameFramework/Entity.hpp"

namespace Xenon
{
	/**
	 * Application class.
	 * This class is to be inherited by the game application.
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
		 * This method is called by the entry point to execute the application.
		 */
		void execute();

	protected:
		/**
		 * On update method.
		 * This method is called to update the game systems.
		 *
		 * @param delta The time difference in microseconds from the last frame to this frame.
		 */
		virtual void onUpdate(std::chrono::microseconds delta) = 0;

	public:
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

		bool m_bShouldRun = true;
	};
}

/**
 * Xenon setup application macro.
 * This macro contains the entry point and other information required to setup the engine's entry point.
 */
#define XENON_SETUP_APPLICATION(object)		\
int main()									\
{											\
	object instance;						\
	instance.execute();						\
											\
	return 0;								\
}											