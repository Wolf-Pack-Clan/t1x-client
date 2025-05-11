#include "shared.h"

#define clc_stringData ((PCHAR)0x495e6f4)
#define clc_stringOffsets ((PINT)0x495c6f4)
#define cs0 (clc_stringData + clc_stringOffsets[0])
#define cs1 (clc_stringData + clc_stringOffsets[1])

#ifndef STOCK_API

#ifdef BUILDING_T1X_DLL
#define STOCK_API __declspec(dllexport)
#else
#define STOCK_API __declspec(dllimport)
#endif

#endif

namespace stock
{
	STOCK_API extern const int MAX_STRING_CHARS;
	STOCK_API extern const int CS_SERVERINFO;
	STOCK_API extern const int CS_SYSTEMINFO;
	
	namespace cgame_mp
	{
		//WEAK adjuster<cvarTable_t> cvarTable{ 0x300749A0, BASE_CGAME_MP };
		STOCK_API extern adjuster<cvarTable_t> cvarTable;
		//WEAK adjuster<int (*)(int arg, ...)> syscall{ 0x30074898, BASE_CGAME_MP };
		STOCK_API extern adjuster<int (*)(int arg, ...)> syscall;
	}

	//WEAK adjuster<int> cmd_argc{ 0x008930f0 };
	STOCK_API extern adjuster<int> cmd_argc;
	//WEAK adjuster<char*> cmd_argv{ 0x00890bf0 };
	STOCK_API extern adjuster<char*> cmd_argv;
	//WEAK adjuster<pmove_t*> pm{ 0x3019d570, BASE_CGAME_MP };
	STOCK_API extern adjuster<pmove_t*> pm;
	//WEAK adjuster<connstate_t> cls_state{ 0x155F2C0 };
	STOCK_API extern adjuster<connstate_t> cls_state;

	//WEAK adjuster<qboolean> refreshActive{ 0x401EA698, BASE_UI_MP };
	STOCK_API extern adjuster<qboolean> refreshActive;






	//WEAK adjuster<vm_t*> uivm{ 0, 0x0161747c };
	//WEAK adjuster<int> ping{ 0x41405d };
	STOCK_API extern adjuster<int> bg_iNumWeapons;
}
