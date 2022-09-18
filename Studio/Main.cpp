// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Common/SystemHandler.hpp"

class TestSystem final : public Xenon::System
{
public:
	TestSystem()
	{
		registerRequestHandler<int>([](int& value) { XENON_LOG_INFORMATION(value); });
		registerRequestHandler<float>([](float& value) { XENON_LOG_INFORMATION(value); });
		registerRequestHandler<double>([](double& value) { XENON_LOG_INFORMATION(value); });
	}

	void onStart() override { XENON_LOG_INFORMATION("On Start Method"); }
	void onUpdate() override { XENON_LOG_INFORMATION("On Update Method"); }
	void onTermination() override { XENON_LOG_INFORMATION("On Termination Method"); }
};

int main()
{
	auto& handler = Xenon::SystemHandler<TestSystem>::Instance();
	handler.issueRequest<int>(100);
	handler.issueRequest<float>(200);
	handler.issueRequest<double>(300);
	handler.terminate();

	return 0;
}