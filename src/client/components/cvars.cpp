#include "pch.h"
#if 1
#include "shared.h"

#include "hook.h"

#include "loader/component_loader.h"

namespace cvars
{
	utils::hook::detour hook_CL_Init;
	utils::hook::detour hook_CG_RegisterCvars;

	stock::cvar_t* r_fullscreen;
	stock::cvar_t* cl_bypassMouseInput;
	stock::cvar_t* cl_allowDownload;
	stock::cvar_t* con_boldgamemessagetime;
	stock::cvar_t* com_sv_running;
	stock::cvar_t* com_timescale;
	stock::cvar_t* sv_cheats;

	namespace vm
	{
		stock::vmCvar_t* cg_drawFPS;
		stock::vmCvar_t* cg_chatHeight;
		stock::vmCvar_t* cg_fov;
		stock::vmCvar_t* cg_lagometer;
	}
	
	static void stub_CL_Init()
	{
		hook_CL_Init.invoke();

		r_fullscreen = stock::Cvar_FindVar("r_fullscreen");
		cl_bypassMouseInput = stock::Cvar_FindVar("cl_bypassMouseInput");
		cl_allowDownload = stock::Cvar_FindVar("cl_allowDownload");
		con_boldgamemessagetime = stock::Cvar_FindVar("con_boldgamemessagetime");
		com_sv_running = stock::Cvar_FindVar("sv_running");
		com_timescale = stock::Cvar_FindVar("timescale");
		sv_cheats = stock::Cvar_FindVar("sv_cheats");
	}

	static void stub_CG_RegisterCvars()
	{
		stock::cgame_mp::cvarTable[4].cvarFlags = stock::CVAR_ARCHIVE;

		hook_CG_RegisterCvars.invoke();

		/*
		TODO: Hook the loop that calls trap_Cvar_Register, to check for cvarName
		so would not need to:
		- know the index values
		- change cvarFlags before calling CG_RegisterCvars
		*/
		vm::cg_fov = stock::cgame_mp::cvarTable[4].vmCvar;
		vm::cg_drawFPS = stock::cgame_mp::cvarTable[11].vmCvar;
		vm::cg_lagometer = stock::cgame_mp::cvarTable[47].vmCvar;
		vm::cg_chatHeight = stock::cgame_mp::cvarTable[83].vmCvar;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			hook_CL_Init.create(0x00411e60, stub_CL_Init);
		}

		void post_cgame() override
		{
			hook_CG_RegisterCvars.create(ABSOLUTE_CGAME_MP(0x300205e0), stub_CG_RegisterCvars);
		}
	};
}

REGISTER_COMPONENT(cvars::component)
#endif