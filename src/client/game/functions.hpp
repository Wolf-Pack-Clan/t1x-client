#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	WEAK symbol<void(float x, float y, int fontID, float scale, float* color, const char* text, float spaceBetweenChars, int maxChars, int arg9)> SCR_DrawString{ 0x004df570 };
	WEAK symbol<cvar_t* (const char* name)> Cvar_FindVar{ 0x00439280 };
	WEAK symbol<cvar_t* (const char* name, const char* value, int flags)> Cvar_Get{ 0x00439350 };
	WEAK symbol<cvar_t* (const char* name, const char* value)> Cvar_Set{ 0x00439650 };
	WEAK symbol<int()> Sys_Milliseconds{ 0, 0x004659d0 };
	WEAK symbol<LRESULT CALLBACK(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> MainWndProc{ 0x466BE0 };
	WEAK symbol<void()> IN_DeactivateMouse{ 0x4616b0 };
	WEAK symbol<void()> IN_ActivateMouse{ 0x461730 };
	WEAK symbol<void(const char* msg, ...)> Com_Printf{ 0x004357b0 };
	WEAK symbol<void()> CG_ServerCommand{ 0x3002e0d0, BASE_CGAME_MP };
	WEAK symbol<void(const char* cmd_name, xcommand_t function)> Cmd_AddCommand{ 0x00428840 };
	WEAK symbol<void(int code, const char* fmt, ...)> Com_Error{ 0x00435ad0 };
	WEAK symbol<void()> IN_MouseMove{ 0x00461850 };
	WEAK symbol<void(int mstate)> IN_MouseEvent{ 0x004617d0 };
	
	


	//WEAK symbol<void()> CL_Disconnect_f{ 0, 0x0040f5f0 };
	//WEAK symbol<void(float x, float y, const char* s, float alpha)> CG_DrawBigString{ 0, 0x30019710, BASE_CGAME_MP };
	//WEAK symbol<int(vm_t* vm, int callnum, ...)> VM_Call{ 0, 0x00460480 };
}