namespace stock
{
	int Cmd_Argc();
	char* Cmd_Argv(int arg);
	weaponInfo_t* BG_GetInfoForWeapon(int weaponNum);
	const char* Info_ValueForKey(const char* buffer, const char* key);
	void CL_MouseEvent(int _dx, int _dy);
	
	WEAK adjuster<void(float x, float y, int fontID, float scale, float* color, const char* text, float spaceBetweenChars, int maxChars, int arg9)> SCR_DrawString{ 0x004df570 };
	WEAK adjuster<cvar_t* (const char* name)> Cvar_FindVar{ 0x00439280 };
	WEAK adjuster<cvar_t* (const char* name, const char* value, int flags)> Cvar_Get{ 0x00439350 };
	WEAK adjuster<cvar_t* (const char* name, const char* value)> Cvar_Set{ 0x00439650 };
	WEAK adjuster<LRESULT CALLBACK(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> MainWndProc{ 0x466BE0 };
	WEAK adjuster<void()> IN_DeactivateMouse{ 0x4616b0 };
	WEAK adjuster<void()> IN_ActivateMouse{ 0x461730 };	
	WEAK adjuster<void(const char* cmd_name, xcommand_t function)> Cmd_AddCommand{ 0x00428840 };
	WEAK adjuster<void(int code, const char* fmt, ...)> Com_Error{ 0x00435ad0 };
	WEAK adjuster<void(cbufExec_t exec_when, const char* text)> Cbuf_ExecuteText{ 0x00428290 };
	WEAK adjuster<const char*(netadr_t a)> NET_AdrToString{ 0x00449150 };
	WEAK adjuster<qboolean(netadr_t a, netadr_t b)> NET_CompareAdr{ 0x00449230 };
	





	//WEAK adjuster<void(float x, float y, const char* s, float alpha)> CG_DrawBigString{ 0x30019710, BASE_CGAME_MP };
	//WEAK adjuster<void(const char* msg, ...)> Com_Printf{ 0x004357b0 };
	//WEAK adjuster<void()> CL_Disconnect_f{ 0x0040f5f0 };
	//WEAK symbol<int(vm_t* vm, int callnum, ...)> VM_Call{ 0, 0x00460480 };
}