// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Studio.hpp"

#include "XenonCore/Logging.hpp"
#include "XenonCore/TaskGraph.hpp"

void run(Xenon::BackendType backend)
{
	auto studio = Studio(backend);
	studio.run();
}

int main()
{
	{
		auto graph = Xenon::TaskGraph(3);
		auto a = graph.create([] { XENON_LOG_INFORMATION("Task: a	Thread: {}", std::hash<std::thread::id>()(std::this_thread::get_id())); });
		auto b = a->after([] { XENON_LOG_INFORMATION("Task: b	Thread: {}", std::hash<std::thread::id>()(std::this_thread::get_id())); });
		auto c = a->after([] { XENON_LOG_INFORMATION("Task: c	Thread: {}", std::hash<std::thread::id>()(std::this_thread::get_id())); });
		auto d = graph.create([] { XENON_LOG_INFORMATION("Task: d	Thread: {}", std::hash<std::thread::id>()(std::this_thread::get_id())); }, { b, c });

		graph.complete();
		XENON_LOG_INFORMATION("-- CLEARNING --");
		
		a->reset();
		b->reset(a);
		c->reset(a);
		d->reset({ b, c });
		auto e = d->after([] { XENON_LOG_INFORMATION("Task: e	Thread: {}", std::hash<std::thread::id>()(std::this_thread::get_id())); });
	}

	run(Xenon::BackendType::Vulkan);
	run(Xenon::BackendType::DirectX_12);

	return 0;
}