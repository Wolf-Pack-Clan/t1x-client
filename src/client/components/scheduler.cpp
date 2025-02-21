#include <pch.hpp>
#if 1
#include <hook.hpp>
#include <concurrency.hpp>
#include <thread.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"

namespace scheduler
{
	namespace
	{
		struct task
		{
			std::function<bool()> handler{};
			std::chrono::milliseconds interval{};
			std::chrono::high_resolution_clock::time_point last_call{};
		};

		using task_list = std::vector<task>;

		class task_pipeline
		{
		public:
			void add(task&& task)
			{
				new_callbacks_.access([&task](task_list& tasks)
				{
					tasks.emplace_back(std::move(task));
				});
			}

			void execute()
			{
				callbacks_.access([&](task_list& tasks)
				{
					this->merge_callbacks();

					for (auto i = tasks.begin(); i != tasks.end();)
					{
						const auto now = std::chrono::high_resolution_clock::now();
						const auto diff = now - i->last_call;

						if (diff < i->interval)
						{
							++i;
							continue;
						}

						i->last_call = now;

						const auto res = i->handler();
						if (res == cond_end)
						{
							i = tasks.erase(i);
						}
						else
						{
							++i;
						}
					}
				});
			}

		private:
			utils::concurrency::container<task_list> new_callbacks_;
			utils::concurrency::container<task_list, std::recursive_mutex> callbacks_;

			void merge_callbacks()
			{
				callbacks_.access([&](task_list& tasks)
				{
					new_callbacks_.access([&](task_list& new_tasks)
					{
						tasks.insert(tasks.end(), std::move_iterator<task_list::iterator>(new_tasks.begin()), std::move_iterator<task_list::iterator>(new_tasks.end()));
						new_tasks = {};
					});
				});
			}
		};

		volatile bool kill = false;
		std::thread thread;
		task_pipeline pipelines[pipeline::count];
		
		utils::hook::detour Com_Frame_hook;
		utils::hook::detour SV_Frame_hook;
		utils::hook::detour RE_EndFrame_hook;
		
		void execute(const pipeline type)
		{
			assert(type >= 0 && type < pipeline::count);
			pipelines[type].execute();
		}
		
		void RE_EndFrame_stub(int* frontEndMsec, int* backEndMsec)
		{
			execute(pipeline::renderer);
			RE_EndFrame_hook.invoke(frontEndMsec, backEndMsec);
		}
		void SV_Frame_stub(int msec)
		{
			execute(pipeline::server);
			SV_Frame_hook.invoke(msec);
		}
		void Com_Frame_stub()
		{
			execute(pipeline::common);
			Com_Frame_hook.invoke();
		}
	}
	
	void schedule(const std::function<bool()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		assert(type >= 0 && type < pipeline::count);

		task task;
		task.handler = callback;
		task.interval = delay;
		task.last_call = std::chrono::high_resolution_clock::now();

		pipelines[type].add(std::move(task));
	}

	void loop(const std::function<void()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		schedule([callback]()
		{
			callback();
			return cond_continue;
		}, type, delay);
	}

	void once(const std::function<void()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		schedule([callback]()
		{
			callback();
			return cond_end;
		}, type, delay);
	}
	
	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			thread = utils::thread::create_named_thread("Async Scheduler", []()
			{
				while (!kill)
				{
					execute(pipeline::async);
					std::this_thread::sleep_for(10ms);
				}
			});
		}

		void post_unpack() override
		{
			Com_Frame_hook.create(reinterpret_cast<void(*)>(0x00437f40), Com_Frame_stub);
			SV_Frame_hook.create(reinterpret_cast<void(*)(int)>(0x0045b1d0), SV_Frame_stub);
			RE_EndFrame_hook.create(reinterpret_cast<void(*)(int*, int*)>(0x004de4b0), RE_EndFrame_stub);
		}
		
		void pre_destroy() override
		{
			kill = true;
			if (thread.joinable())
				thread.join();
		}
	};
}

REGISTER_COMPONENT(scheduler::component)
#endif