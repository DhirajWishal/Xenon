// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "TestContainer.hpp"

#include <iostream>
#include <chrono>

bool TestContainer::test()
{
	std::cout << "Xenon Test Container" << std::endl;
	bool failed = false;

	// Iterate over the tests and execute them.
	for (auto& pTest : m_Tests)
	{
		std::cout << "------------------------------------------------------------" << std::endl;
		std::cout << "Executing test: " << pTest->getTitle() << std::endl;
		const auto startTime = std::chrono::high_resolution_clock::now();

		// Try and run the test. 
		try
		{
			std::cout << std::endl;
			const auto returnCode = pTest->onExecute();
			std::cout << std::endl;

			failed |= returnCode > 0;

			std::cout << "Execution ended with return code: " << returnCode << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << "Execution failed; exception thrown: " << e.what() << std::endl;
			failed |= true;
		}

		// Log the time taken to execute.
		std::cout << "Time taken: " << std::chrono::high_resolution_clock::now() - startTime << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl;
	}

	return failed;
}
