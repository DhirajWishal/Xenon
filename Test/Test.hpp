// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

/**
 * Test class.
 * All the tests of this engine will be inherited by this class and will be executed from the test container.
 */
class Test
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param title The title of the test.
	 */
	explicit Test(const std::string& title) : m_Title(title) {}

	/**
	 * Default virtual destructor.
	 */
	virtual ~Test() = default;

	/**
	 * Virtual on execute function.
	 * This method should contain the actual test code.
	 *
	 * @return The execution result. 0 means success and anything else means error.
	 */
	[[nodiscard]] virtual int32_t onExecute() = 0;

public:
	/**
	 * Get the test title.
	 *
	 * @return The title string view.
	 */
	[[nodiscard]] std::string_view getTitle() const { return m_Title; }

private:
	std::string m_Title;
};