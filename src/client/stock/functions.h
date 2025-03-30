namespace stock
{
	int Cmd_Argc();
	char* Cmd_Argv(int arg);
	//WEAK adjuster<void(int code, const char* fmt, ...)> Com_Error{ 0x00435ad0 };
	//WEAK adjuster<void(cbufExec_t exec_when, const char* text)> Cbuf_ExecuteText{ 0x00428290 };

	WEAK adjuster<void(const char* cmd_name, xcommand_t function)> Cmd_AddCommand{ 0x0042c3b0 }; // address updated





	//WEAK adjuster<void(float x, float y, float width, float height, qhandle_t hShader)> SCR_DrawPic{ 0x004168d0 };
	//WEAK adjuster<void(float x, float y, const char* s, float alpha)> CG_DrawBigString{ 0x30019710, BASE_CGAME_MP };
	//WEAK adjuster<void(const char* msg, ...)> Com_Printf{ 0x004357b0 };
	//WEAK adjuster<void()> CL_Disconnect_f{ 0x0040f5f0 };
	//WEAK symbol<int(vm_t* vm, int callnum, ...)> VM_Call{ 0, 0x00460480 };
}
