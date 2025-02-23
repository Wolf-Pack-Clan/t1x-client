#include "pch.h"
#if 1
#include "shared.h"

#include "hook.h"

#include "loader/component_loader.h"

namespace test
{
	static void Command_test()
	{
#if 0



		std::stringstream ss;
		stock::cvarTable_t* cv;
		int i;
		auto cvarTable = stock::cgame_mp::cvarTable;
		for (i = 0, cv = cvarTable; i < 0xad; i++, cv++)
		{
			ss << "####### cvarName: " << cv->cvarName << ", i = " << i << std::endl;
		}
		OutputDebugString(ss.str().c_str());



#endif
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