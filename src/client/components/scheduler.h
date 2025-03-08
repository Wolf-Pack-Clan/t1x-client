#pragma once

#include "shared.h"

#include "hook.h"
#include "concurrency.h"
#include "thread.h"

#include "loader/component_loader.h"

namespace scheduler
{
	using namespace std::literals;

	enum pipeline
	{
		client,
		server,
		cgame,
		renderer,
		async,
		count
	};

	static const bool cond_continue = false;
	static const bool cond_end = true;

	void schedule(const std::function<bool()>& callback, pipeline type = pipeline::async, std::chrono::milliseconds delay = 0ms);
	void loop(const std::function<void()>& callback, pipeline type = pipeline::async, std::chrono::milliseconds delay = 0ms);
	void once(const std::function<void()>& callback, pipeline type = pipeline::async, std::chrono::milliseconds delay = 0ms);
}