#include "pch.h"
#if 1
#include "movement.h"

namespace movement
{	
	static void Cmd_Test()
	{
		printf("############# HELLO\n");
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{	
			stock::Cmd_AddCommand("test", Cmd_Test);
		}

		void post_cgame() override
		{
			//utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032fe8), stub_cg_zoomSensitivity_calculation);
			//utils::hook::jump(ABSOLUTE_CGAME_MP(0x3000d82b), stub_PM_StepSlideMove_PM_ClipVelocity);
		}
	};
}

REGISTER_COMPONENT(movement::component)
#endif