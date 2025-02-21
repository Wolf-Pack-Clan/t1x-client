#include <pch.hpp>
#if 1
#include <shared.hpp>
#include <hook.hpp>
#include "loader/component_loader.hpp"

#include "window.hpp"

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