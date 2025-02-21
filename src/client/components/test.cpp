#include "pch.h"
#if 1
#include "shared.h"
#include "hook.h"
#include "loader/component_loader.h"

#include "window.h"
#include "scheduler.h"

namespace test
{
	static void Command_test()
	{
		window::MSG("test");

	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			stock::Cmd_AddCommand("test", Command_test);




		}
	};
}

REGISTER_COMPONENT(test::component)
#endif