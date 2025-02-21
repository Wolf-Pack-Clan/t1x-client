#include "pch.h"

#include "shared.h"

namespace stock
{
	uintptr_t addr_CL_MouseEvent = 0x0040b0a0;

	int Cmd_Argc()
	{
		return *cmd_argc;
	}
	
	char* Cmd_Argv(int arg)
	{
		return cmd_argv[arg];
	}
	
	weaponInfo_t* BG_GetInfoForWeapon(int weaponNum)
	{
		auto cg_weapons_ptr = *reinterpret_cast<uintptr_t*>(*cg_weapons);
		return reinterpret_cast<weaponInfo_t**>(cg_weapons_ptr)[weaponNum];
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
	
	void CL_MouseEvent(int _dx, int _dy)
	{
		_asm
		{
			mov ecx, _dx;
			push eax;
			mov eax, _dy;
			call addr_CL_MouseEvent;
			add esp, 4;
		}
	}
}