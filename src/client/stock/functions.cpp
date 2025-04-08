#include "pch.h"

#include "shared.h"

namespace stock
{

	int Cmd_Argc()
	{
		return *cmd_argc;
	}
	
	char* Cmd_Argv(int arg)
	{
		return cmd_argv[arg];
	}

	const char* Info_ValueForKey(const char* buffer, const char* key)
	{
		_asm
		{
			mov ebx, key;
			mov ecx, buffer;
			mov eax, 0x0044fb70;
			call eax;
		}
	}

	int32_t BG_GetNumWeapons()
	{
		return bg_iNumWeapons != nullptr ? *bg_iNumWeapons : 0;
	}

	bool BG_IsWeaponIndexValid(int32_t index)
	{
		return address_cgame_mp > 0 && index > 0 && index <= BG_GetNumWeapons();
	}

	int32_t BG_GetWeaponIndexForName(const char* name)
	{
		printf("#### BG_GetWeaponIndexForName: %s\n", name);
		printf("#### BG_GetWeaponIndexForName: address_cgame_mp = 0x%08X\n", (uint32_t)address_cgame_mp);
		_asm
		{
			mov esi, name
			mov eax, address_cgame_mp
			add eax, 0x00011010
			call eax
		}
		printf("#### BG_GetWeaponIndexForName: done\n");
	}

	weaponinfo_t* BG_GetWeaponDef(int32_t index)
	{
		weaponinfo_t* weaponinfo = nullptr;

		if (BG_IsWeaponIndexValid(index))
		{
			uintptr_t ptr = *(uintptr_t*)(address_cgame_mp + 0x0010ed40);
			weaponinfo = *(weaponinfo_t**)(ptr + index * 4);
		}

		return weaponinfo;
	}
}