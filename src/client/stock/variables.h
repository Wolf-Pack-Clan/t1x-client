
namespace stock
{
	constexpr auto MAX_STRING_CHARS = 1024;
	constexpr auto CS_SERVERINFO = 0;
	constexpr auto CS_SYSTEMINFO = 1;
	
	namespace cgame_mp
	{
		//WEAK adjuster<cvarTable_t> cvarTable{ 0x300749A0, BASE_CGAME_MP };
		WEAK adjuster<cvarTable_t> cvarTable{ 0x300851f8, BASE_CGAME_MP };
		//WEAK adjuster<int (*)(int arg, ...)> syscall{ 0x30074898, BASE_CGAME_MP };
		WEAK adjuster<int (*)(int arg, ...)> syscall{ 0x30085e9c, BASE_CGAME_MP };
	}

	//WEAK adjuster<int> cmd_argc{ 0x008930f0 };
	WEAK adjuster<int> cmd_argc{ 0x008930f0 };
	//WEAK adjuster<char*> cmd_argv{ 0x00890bf0 };
	WEAK adjuster<char*> cmd_argv{ 0x00890bf0 };
	//WEAK adjuster<pmove_t*> pm{ 0x3019d570, BASE_CGAME_MP };
	WEAK adjuster<pmove_t*> pm{ 0x30539850, BASE_CGAME_MP };
	//WEAK adjuster<connstate_t> cls_state{ 0x155F2C0 };
	WEAK adjuster<connstate_t> cls_state{ 0x004056be };

	//WEAK adjuster<qboolean> refreshActive{ 0x401EA698, BASE_UI_MP };
	WEAK adjuster<qboolean> refreshActive{ 0x40223d74, BASE_UI_MP };






	//WEAK adjuster<vm_t*> uivm{ 0, 0x0161747c };
	//WEAK adjuster<int> ping{ 0x41405d };
}
