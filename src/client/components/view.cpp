#include "pch.h"
#if 1
#include "view.h"

namespace view
{
	stock::cvar_t* cg_fovScaleEnable;
	stock::cvar_t* cg_fovScale;
	stock::cvar_t* record_respawn;

	utils::hook::detour hook_CG_Respawn;
	utils::hook::detour hook_CL_PlayDemo_f;
	
	static float scaledFOV(float fov)
	{
		int* flag = (int*)ABSOLUTE_CGAME_MP(0x302071dc); // Might be cg.snap->ps.eFlags
		if (*flag & stock::EF_MG42_ACTIVE)
			return 55;

		if (cg_fovScaleEnable->integer)
			return fov * cg_fovScale->value;

		return fov;
	}
	
	static __declspec(naked) void CG_CalcFov_return_stub()
	{
		__asm
		{
			sub esp, 4;
			fstp dword ptr[esp];			
			push dword ptr[esp];
			call scaledFOV;
			add esp, 4;

			fstp dword ptr[esp];
			fld dword ptr[esp];
			add esp, 4;

			pop ecx;
			ret;
		}
	}
	
	static void stub_CG_Respawn()
	{
		hook_CG_Respawn.invoke();

		if (record_respawn->integer)
		{
			if (*stock::clc_demoplaying)
				return;
			
			auto unknown = *(int*)(*(int*)ABSOLUTE_CGAME_MP(0x301E2160) + 0x18); // Seems to be/related to cg.snap->ps.pm_flags
			if (unknown == 0x40000) // Is 0x10000 when following
			{
				stock::Cbuf_ExecuteText(stock::EXEC_APPEND, "record\n");
			}
		}
	}

	static void stub_CL_PlayDemo_f()
	{
		hook_CL_PlayDemo_f.invoke();
		stock::Cvar_Set("sv_cheats", "1");
	}

	static void death_stoprecord()
	{
		if (record_respawn->integer)
		{
			if (*stock::clc_demorecording)
			{
				if ((*stock::pm)->ps->stats[stock::STAT_HEALTH] == 0)
				{
					stock::Cbuf_ExecuteText(stock::EXEC_APPEND, "stoprecord\n");
				}
			}
		}
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			cg_fovScaleEnable = stock::Cvar_Get("cg_fovScaleEnable", "0", stock::CVAR_ARCHIVE);
			cg_fovScale = stock::Cvar_Get("cg_fovScale", "1", stock::CVAR_ARCHIVE);
			record_respawn = stock::Cvar_Get("record_respawn", "0", stock::CVAR_ARCHIVE);

			hook_CL_PlayDemo_f.create(0x0040eb40, stub_CL_PlayDemo_f);
		}

		void post_cgame() override
		{
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032f2a), CG_CalcFov_return_stub);
			
			hook_CG_Respawn.create(ABSOLUTE_CGAME_MP(0x30028a70), stub_CG_Respawn);

			scheduler::loop(death_stoprecord, scheduler::pipeline::client);
		}
	};
}

REGISTER_COMPONENT(view::component)
#endif