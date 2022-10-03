// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Test.hpp"

#include <vector>
#include <memory>

/**
 * Test container class.
 * This utility class contains all the tests that are required to be run by the test project.
 * To execute the tests, call the test() method.
 */
class TestContainer final
{
public:
	/**
	 * Default constructor.
	 */
	TestContainer() = default;

	/**
	 * Insert a new test to the container.
	 *
	 * @tparam Type The test type.
	 * @tparam Arguments The constructor arguments.
	 * @param arguments The arguments to construct the test.
	 * @return The created test pointer.
	 */
	template<class Type, class... Arguments>
	Type* insert(Arguments&&... arguments) { return static_cast<Type*>(m_Tests.emplace_back(std::make_unique<Type>(std::forward<Arguments>(arguments)...)).get()); }

	/**
	 * Test all the tests.
	 *
	 * @return True if there was an error with at least one test.
	 * @return False if everything went okay.
	 */
	[[nodiscard]] bool test();

private:
	std::vector<std::unique_ptr<Test>> m_Tests;
};