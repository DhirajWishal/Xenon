// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>

namespace Xenon
{
	/**
	 * Counting fence class.
	 * This class can be used to wait till multiple worker threads have finished execution. This works much like a std::latch but can be reused.
	 */
	class CountingFence final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param initialValue The initial value to initialize with. Default is 0.
		 */
		explicit CountingFence(uint64_t initialValue = 0) : m_Counter(initialValue) {}

		/**
		 * Decrement the internal counter by a given value.
		 *
		 * @param decrement The value to decrement the counter with. Default is 1.
		 */
		void arrive(uint64_t decrement = 1);

		/**
		 * Wait till the counter has reached 0.
		 */
		void wait() const;

		/**
		 * Reset the counter.
		 *
		 * @param value The value to reset with.
		 */
		void reset(uint64_t value);

	private:
		std::atomic_uint64_t m_Counter;
	};
}