namespace stock
{
	constexpr auto MAX_STRING_CHARS = 1024;
	constexpr auto CVAR_ARCHIVE = 1;
	constexpr auto CVAR_LATCH = 32;
	constexpr auto KEYCATCH_CONSOLE = 0x0001;
	constexpr auto KEYCATCH_UI = 0x0002;
	constexpr auto KEYCATCH_MESSAGE = 0x0004;
	constexpr auto KEYCATCH_CGAME = 0x0008;
	constexpr auto EF_MG42_ACTIVE = 0xc000;
	constexpr auto YAW = 1;
	
	namespace cgame_mp
	{
		WEAK adjuster<cvarTable_t> cvarTable{ 0x300749A0, BASE_CGAME_MP };
	}

	WEAK adjuster<int> cls_keyCatchers{ 0x0155f2c4 };
	WEAK adjuster<HWND> hWnd{ 0x16C35E8 };
	WEAK adjuster<int> cmd_argc{ 0x008930f0 };
	WEAK adjuster<char*> cmd_argv{ 0x00890bf0 };
	WEAK adjuster<float> viewangles{ 0x0143a9a0 };
	WEAK adjuster<float> cg_zoomSensitivity{ 0x3020b5f4, BASE_CGAME_MP };
	WEAK adjuster<float> ads_progress{ 0x30207214, BASE_CGAME_MP }; // Between 0 and 1, might be cg.zoomTime
	WEAK adjuster<float> fov_visible{ 0x3020958c, BASE_CGAME_MP }; // Not the cg_fov cvar value, might be cg.zoomval
	WEAK adjuster<pmove_t*> pm{ 0x3019d570, BASE_CGAME_MP };
	WEAK adjuster<weaponInfo_t[]> cg_weapons{ 0x300eef3c, BASE_CGAME_MP };
	WEAK adjuster<qboolean> refreshActive{ 0x401EA698, BASE_UI_MP };
	WEAK adjuster<qboolean> clc_demoplaying{ 0x015ef004 };
	WEAK adjuster<vm_t*> cgvm{ 0x01617348 };
	WEAK adjuster<qboolean> mouseActive{ 0x8e2520 };
	WEAK adjuster<qboolean> mouseInitialized{ 0x8e2524 };
	WEAK adjuster<connstate_t> cls_state{ 0x155F2C0 };






	//WEAK adjuster<int> ping{ 0x41405d };
	//WEAK symbol<vm_t*> uivm{ 0, 0x0161747c };
}

namespace cvars
{
	extern stock::cvar_t* r_fullscreen;
	extern stock::cvar_t* cl_bypassMouseInput;
	extern stock::cvar_t* cl_allowDownload;
	extern stock::cvar_t* con_boldgamemessagetime;
	extern stock::cvar_t* com_sv_running;

	namespace vm
	{
		extern stock::vmCvar_t* cg_lagometer;
		extern stock::vmCvar_t* cg_fov;
		extern stock::vmCvar_t* cg_drawFPS;
		extern stock::vmCvar_t* cg_chatHeight;
	}
}