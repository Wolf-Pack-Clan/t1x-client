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
}