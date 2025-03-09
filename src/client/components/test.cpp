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


#endif
	}





#if 0
	utils::hook::detour hook_Test;
	static void stub_Test()
	{
		OutputDebugString("stub_Test\n");
		hook_Test.invoke();

	}
#endif



	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			stock::Cmd_AddCommand("test", Command_test);

			//hook_Test.create(0x0040eb40, stub_Test);

		}
		
		void post_cgame() override
		{
			//hook_Test.create(ABSOLUTE_CGAME_MP(0x30028a70), stub_Test);






		}
	};
}

REGISTER_COMPONENT(test::component)
#endif