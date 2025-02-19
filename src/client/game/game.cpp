#include <std_include.hpp>
#include "game.hpp"

namespace game
{
	int Cmd_Argc()
	{
		return *cmd_argc;
	}
	
	char* Cmd_Argv(int arg)
	{
		return cmd_argv[arg];
	}
	
	game::weaponInfo_t* BG_GetInfoForWeapon(int weaponNum)
	{
		auto cg_weapons_ptr = *reinterpret_cast<uintptr_t*>(*game::cg_weapons);
		return reinterpret_cast<game::weaponInfo_t**>(cg_weapons_ptr)[weaponNum];
	}
	
	const char* Info_ValueForKey(const char* buffer, const char* key)
	{
		_asm
		{
			mov ebx, key;
			mov ecx, buffer;

			mov eax, 0x0044ada0;
			call eax;
		}
	}
	
	namespace environment
	{
		bool mohaa = false;
		std::string get_client_filename()
		{
			if (mohaa)
				return "mohaa.exe";
			else
				return "CoDMP.exe";
		}
	}
}