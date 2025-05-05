#include "pch.h"
#if 1
#include "shared.h"

#include "hook.h"

#include "loader/component_loader.h"
#include "test.h"

namespace test
{
	static void Command_gosha()
	{
		printf("%s\n", sgosha);
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
			stock::Cmd_AddCommand("gosha", Command_gosha);

			//hook_Test.create(0x0040eb40, stub_Test);

		}
		
		void post_cgame() override
		{
			//hook_Test.create(ABSOLUTE_CGAME_MP(0x30028a70), stub_Test);
			printf("test post_cgame called");
		}
		void post_ui_mp() override
        {
        	printf("test post_ui_mp called");
        }
	};
}

extern "C" __declspec(dllexport) component_interface* create_component()
{
    printf("Creating test component\n");
    return new test::component();
}
#endif
