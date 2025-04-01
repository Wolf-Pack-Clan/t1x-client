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
}