#ifndef STOCK_FUNCTIONS_H
#define STOCK_FUNCTIONS_H

#ifndef STOCK_API

#ifdef BUILDING_T1X_DLL
#define STOCK_API __declspec(dllexport)
#else
#define STOCK_API __declspec(dllimport)
#endif

#endif

namespace stock
{
	STOCK_API int Cmd_Argc();
	STOCK_API char* Cmd_Argv(int arg);
	STOCK_API const char* Info_ValueForKey(const char* buffer, const char* key);
	//WEAK adjuster<void(int code, const char* fmt, ...)> Com_Error{ 0x00435ad0 };
	//WEAK adjuster<void(cbufExec_t exec_when, const char* text)> Cbuf_ExecuteText{ 0x00428290 };

	STOCK_API extern adjuster<void(const char* cmd_name, xcommand_t function)> Cmd_AddCommand; // address updated





	//WEAK adjuster<void(float x, float y, float width, float height, qhandle_t hShader)> SCR_DrawPic{ 0x004168d0 };
	//WEAK adjuster<void(float x, float y, const char* s, float alpha)> CG_DrawBigString{ 0x30019710, BASE_CGAME_MP };
	//WEAK adjuster<void(const char* msg, ...)> Com_Printf{ 0x004357b0 };
	//WEAK adjuster<void()> CL_Disconnect_f{ 0x0040f5f0 };
	//WEAK symbol<int(vm_t* vm, int callnum, ...)> VM_Call{ 0, 0x00460480 };
	STOCK_API int32_t BG_GetNumWeapons();
	STOCK_API bool BG_IsWeaponIndexValid(int32_t index);
	STOCK_API int32_t BG_GetWeaponIndexForName(const char* name);
	STOCK_API weaponinfo_t *BG_GetWeaponDef(int32_t index);
}

#endif
