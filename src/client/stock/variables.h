namespace stock
{
	constexpr auto MAX_STRING_CHARS = 1024;
	constexpr auto MAX_WEAPONS = 64;

	constexpr auto CVAR_ARCHIVE = 1;
	constexpr auto CVAR_LATCH = 32;

	constexpr auto KEYCATCH_CONSOLE = 0x0001;
	constexpr auto KEYCATCH_UI = 0x0002;
	constexpr auto KEYCATCH_MESSAGE = 0x0004;
	constexpr auto KEYCATCH_CGAME = 0x0008;

	constexpr auto YAW = 1;

	constexpr auto EF_MG42_ACTIVE = 0xc000;

	WEAK adjuster<int> cls_keyCatchers{ 0x0155f2c4 };
	WEAK adjuster<HWND> hWnd{ 0x16C35E8 };
	WEAK adjuster<int> cmd_argc{ 0x008930f0 };
	WEAK adjuster<char*> cmd_argv{ 0x00890bf0 };
	WEAK adjuster<float> viewangles{ 0x0143a9a0 };
	WEAK adjuster<float> cg_zoomSensitivity{ 0x3020b5f4, BASE_CGAME_MP };
	WEAK adjuster<float> ads_progress{ 0x30207214, BASE_CGAME_MP }; // Between 0 and 1
	WEAK adjuster<float> fov_visible{ 0x3020958c, BASE_CGAME_MP }; // Not the cg_fov cvar value
	WEAK adjuster<float> cg_fov_value{ 0x30298c68, BASE_CGAME_MP };
	WEAK adjuster<pmove_t*> pm{ 0x3019d570, BASE_CGAME_MP };
	WEAK adjuster<weaponInfo_t[MAX_WEAPONS]> cg_weapons{ 0x300eef3c, BASE_CGAME_MP };
	WEAK adjuster<qboolean> refreshActive{ 0x401EA698, BASE_UI_MP };
	WEAK adjuster<qboolean> clc_demoplaying{ 0x015ef004 };
	WEAK adjuster<vm_t*> cgvm{ 0x01617348 };
	WEAK adjuster<qboolean> mouseActive{ 0x8e2520 };
	WEAK adjuster<qboolean> mouseInitialized{ 0x8e2524 };





	//WEAK adjuster<int> ping{ 0x41405d };
	//WEAK symbol<connstate_t> cls_state{ 0, 0x155F2C0 };
	//WEAK symbol<vm_t*> uivm{ 0, 0x0161747c };
}