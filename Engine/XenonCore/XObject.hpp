// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "JobSystem.hpp"

namespace Xenon
{
	/**
	 * Xenon object class.
	 * This is the base class for all the Xenon objects.
	 */
	class XObject
	{
	public:
		/**
		 * Default constructor.
		 */
		XObject() = default;

		/**
		 * Default virtual destructor.
		 */
		virtual ~XObject() = default;

	public:
		/**
		 * Get the internal global job system.
		 *
		 * @return The job system reference.
		 */
		[[nodiscard]] static JobSystem& GetJobSystem();

	private:
		static JobSystem m_JobSystem;
	};
}