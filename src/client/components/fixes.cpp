#include "pch.h"
#if 1
#include "shared.h"

#include "hook.h"

#include "loader/component_loader.h"

namespace fixes
{
	utils::hook::detour hook_UI_StartServerRefresh;
	utils::hook::detour hook_CL_Disconnect;

	uintptr_t pfield_charevent_return = 0x40CB77;
	uintptr_t pfield_charevent_continue = 0x40CB23;
	static __declspec(naked) void stub_Field_CharEvent_ignore_console_char()
	{
		// See https://github.com/xtnded/codextended-client/blob/45af251518a390ab08b1c8713a6a1544b70114a1/cl_input.cpp#L77

		__asm
		{
			cmp ebx, 0x20;
			jge check;
			jmp pfield_charevent_return;

		check:
			cmp ebx, 0x7E;
			jl checked;
			jmp pfield_charevent_return;

		checked:
			jmp pfield_charevent_continue;
		}
	}

	static void stub_UI_StartServerRefresh(stock::qboolean full)
	{
		if (*stock::refreshActive)
			return;
		hook_UI_StartServerRefresh.invoke(full);
	}

	static char* stub_CL_SetServerInfo_hostname_strncpy(char* dest, const char* src, int destsize)
	{
#pragma warning(push)
#pragma warning(disable: 4996)
		strncpy(dest, utils::string::clean(src, false).c_str(), destsize); // destsize is already max-1 (=31), so not using _TRUNCATE, not to lose a char
#pragma warning(pop)
		return dest;
	}
	
	static void stub_CL_Disconnect(stock::qboolean showMainMenu)
	{
		stock::Cvar_Set("timescale", "1");
		hook_CL_Disconnect.invoke(showMainMenu);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{

			/*
			Prevent the CD Key error when joining a server (occurs when joined a fs_game server previously)
			("CD Key is not valid. Please enter...")
			See https://github.com/xtnded/codextended-client/blob/45af251518a390ab08b1c8713a6a1544b70114a1/fixes.cpp#L21
			*/
			utils::hook::nop(0x0042d122, 5);

			// Prevent displaying squares in server name (occurs when hostname contains e.g. SOH chars)
			utils::hook::call(0x412A2C, stub_CL_SetServerInfo_hostname_strncpy);

			// Prevent inserting the char of the console key in the text field (e.g. Superscript Two gets inserted using french keyboard)
			utils::hook::jump(0x40CB1E, stub_Field_CharEvent_ignore_console_char);

			// Prevent timescale remaining modified after leaving server/demo
			hook_CL_Disconnect.create(0x0040ef90, stub_CL_Disconnect);
		}

		void post_ui_mp() override
		{
			// Prevent displaying servers twice (occurs if double click Refresh List)
			hook_UI_StartServerRefresh.create(ABSOLUTE_UI_MP(0x4000ea90), stub_UI_StartServerRefresh);
		}
	};
}

REGISTER_COMPONENT(fixes::component)
#endif