#include "pch.h"
#if 1
#include "security.h"

#include "window.h"

namespace security
{
	stock::cvar_t* cl_allowDownload;
	utils::hook::detour CG_ServerCommand_hook;
	utils::hook::detour CL_SystemInfoChanged_hook;
	
	std::vector<std::string> cvarsWritable_whiteList =
	{
		"g_scriptMainMenu",
		"scr_showweapontab",
		"cg_objectiveText",
		"fs_game",
		"sv_cheats",
		"sv_serverid",
		"timescale",
	};

	static bool cvarIsInWhitelist(const char* cvar_name)
	{
		for (const auto& str : cvarsWritable_whiteList)
			if (!_stricmp(str.c_str(), cvar_name))
				return true;
		return false;
	}

	static void CG_ServerCommand_stub()
	{
		auto cmd = stock::Cmd_Argv(0);
		if (*cmd == 'v')
		{
			auto cvar_name = stock::Cmd_Argv(1);
#if 0
			std::stringstream ss;
			ss << "####### CG_ServerCommand_stub: " << cvar_name << std::endl;
			OutputDebugString(ss.str().c_str());
#endif

			if (!cvarIsInWhitelist(cvar_name))
				return;
		}
		CG_ServerCommand_hook.invoke();
	}

	static void CL_SystemInfoChanged_Cvar_Set_stub(const char* name, const char* value)
	{
#if 0
		std::stringstream ss;
		ss << "####### CL_SystemInfoChanged_Cvar_Set_stub: " << name << "\n";
		OutputDebugString(ss.str().c_str());
#endif

		if (!cvarIsInWhitelist(name))
			return;
		stock::Cvar_Set(name, value);
	}

	static void CL_SystemInfoChanged_stub()
	{
		char* cl_gameState_stringData = (char*)0x01436a7c;
		int* cl_gameState_stringOffsets = (int*)0x1434A7C;
		char* systemInfo = cl_gameState_stringData + cl_gameState_stringOffsets[stock::CS_SYSTEMINFO];
		const char* sv_pakNames = stock::Info_ValueForKey(systemInfo, "sv_pakNames");
		const char* sv_referencedPakNames = stock::Info_ValueForKey(systemInfo, "sv_referencedPakNames");
		
		if (strstr(sv_pakNames, "@") || strstr(sv_referencedPakNames, "@"))
		{
			//stock::Cbuf_ExecuteText(stock::EXEC_APPEND, "disconnect\n");
			//stock::CL_Disconnect_f();
			//window::MSG("Non-pk3 download protection triggered", MB_ICONEXCLAMATION);
			//return;

			stock::Com_Error(stock::ERR_DROP, "Non-pk3 download protection triggered");
		}

		CL_SystemInfoChanged_hook.invoke();
	}

	bool escape_aborted_connection()
	{
		if (*stock::cls_state > stock::CA_DISCONNECTED && *stock::cls_state < stock::CA_ACTIVE)
		{
			stock::Cbuf_ExecuteText(stock::EXEC_APPEND, "disconnect\n");
			return true;
		}
		return false;
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Use a cvar whitelist for CL_SystemInfoChanged
			utils::hook::call(0x00415ffe, CL_SystemInfoChanged_Cvar_Set_stub);
			
			// Check in sv_pakNames and sv_referencedPakNames for an indicator of a non-pk3 file incoming download
			CL_SystemInfoChanged_hook.create(0x00415eb0, CL_SystemInfoChanged_stub);
		}

		void post_cgame() override
		{
			// Use a cvar whitelist for setClientCvar GSC method
			CG_ServerCommand_hook.create(ABSOLUTE_CGAME_MP(0x3002e0d0), CG_ServerCommand_stub);
		}
	};
}

REGISTER_COMPONENT(security::component)
#endif