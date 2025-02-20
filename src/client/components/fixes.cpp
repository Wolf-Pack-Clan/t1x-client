#include <std_include.hpp>
#if 1
#include <hook.hpp>
#include "loader/component_loader.hpp"
#include "stock/game.hpp"

namespace fixes
{
	utils::hook::detour UI_StartServerRefresh_hook;
	
	uintptr_t pfield_charevent_return = 0x40CB77;
	uintptr_t pfield_charevent_continue = 0x40CB23;
	static __declspec(naked) void Field_CharEvent_ignore_console_char_stub()
	{
		// See https://github.com/xtnded/codextended-client/blob/45af251518a390ab08b1c8713a6a1544b70114a1/cl_input.cpp#L77
		
		__asm
		{
			cmp ebx, 20h;
			jge check;
			jmp pfield_charevent_return;
		
		check:
			cmp ebx, 126;
			jl checked;
			jmp pfield_charevent_return;
		
		checked:
			jmp pfield_charevent_continue;
		}
	}
	
	static void UI_StartServerRefresh_stub(game::qboolean full)
	{
		if (*game::refreshActive)
			return;
		UI_StartServerRefresh_hook.invoke(full);
	}

	static char* Q_CleanStr_keep_colors(char* string)
	{
		char* d;
		char* s;
		int c;
		s = string;
		d = string;
		while ((c = *s) != 0)
		{
			if (c >= 0x20 && c <= 0x7E)
			{
#pragma warning(push)
#pragma warning(disable: 4244)
				* d++ = c;
#pragma warning(pop)
			}
			s++;
		}
		*d = '\0';
		return string;
	}
	static char* CL_SetServerInfo_hostname_strncpy_stub(char* dest, const char* src, int destsize)
	{
		char hostname[MAX_STRING_CHARS];
		strncpy_s(hostname, sizeof(hostname), src, _TRUNCATE);
		Q_CleanStr_keep_colors(hostname);
#pragma warning(push)
#pragma warning(disable: 4996)
		strncpy(dest, hostname, destsize); // destsize is already max-1 (=31), so not using _TRUNCATE, not to lose a char
#pragma warning(pop)
		return dest;
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Prevent inserting the char of the console key in the text field (e.g. Superscript Two gets inserted using french keyboard)
			utils::hook::jump(0x40CB1E, Field_CharEvent_ignore_console_char_stub);

			// Prevent displaying squares in server name (occurs when hostname contains e.g. SOH chars)
			utils::hook::call(0x412A2C, CL_SetServerInfo_hostname_strncpy_stub);

			/*
			Prevent the CD Key error when joining a server (occurs when joined a fs_game server previously)
			("CD Key is not valid. Please enter...")
			See https://github.com/xtnded/codextended-client/blob/45af251518a390ab08b1c8713a6a1544b70114a1/fixes.cpp#L21
			*/
			utils::hook::nop(0x0042d122, 5);
		}
		
		void post_ui_mp() override
		{
			// Prevent displaying servers twice (occurs if double click Refresh List)
			UI_StartServerRefresh_hook.create(ABSOLUTE_UI_MP(0x4000ea90), UI_StartServerRefresh_stub);
		}
	};
}

REGISTER_COMPONENT(fixes::component)
#endif