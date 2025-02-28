#include "pch.h"
#if 1
#include "security.h"

#include "window.h"

namespace security
{
	stock::cvar_t* cl_allowDownload;
	utils::hook::detour hook_CG_ServerCommand;
	utils::hook::detour hook_CL_NextDownload;
	
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

	static void stub_CG_ServerCommand()
	{
		auto cmd = stock::Cmd_Argv(0);
		if (*cmd == 'v')
		{
			auto cvar_name = stock::Cmd_Argv(1);
#if 0
			std::stringstream ss;
			ss << "####### stub_CG_ServerCommand: " << cvar_name << std::endl;
			OutputDebugString(ss.str().c_str());
#endif

			if (!cvarIsInWhitelist(cvar_name))
				return;
		}
		hook_CG_ServerCommand.invoke();
	}

	static void stub_CL_SystemInfoChanged_Cvar_Set(const char* name, const char* value)
	{
#if 0
		std::stringstream ss;
		ss << "####### stub_CL_SystemInfoChanged_Cvar_Set: " << name << std::endl;
		OutputDebugString(ss.str().c_str());
#endif

		if (!cvarIsInWhitelist(name))
			return;
		stock::Cvar_Set(name, value);
	}

	static void stub_CL_NextDownload()
	{
		if (!stock::NET_CompareAdr(*stock::cls_autoupdateServer, *stock::clc_serverAddress))
		{
			char* cl_gameState_stringData = (char*)0x01436a7c;
			int* cl_gameState_stringOffsets = (int*)0x1434A7C;
			char* systemInfo = cl_gameState_stringData + cl_gameState_stringOffsets[stock::CS_SYSTEMINFO];
			
			const char* sv_pakNames = stock::Info_ValueForKey(systemInfo, "sv_pakNames");
			const char* sv_referencedPakNames = stock::Info_ValueForKey(systemInfo, "sv_referencedPakNames");

			if (strstr(sv_pakNames, "@") || strstr(sv_referencedPakNames, "@"))
			{
				stock::Cbuf_ExecuteText(stock::EXEC_APPEND, "disconnect\n");
				window::MSG("Non-pk3 download protection triggered", MB_ICONWARNING);
			}
		}

		hook_CL_NextDownload.invoke();
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
			utils::hook::call(0x00415ffe, stub_CL_SystemInfoChanged_Cvar_Set);
			
			// Check in sv_pakNames and sv_referencedPakNames for an indicator of a non-pk3 file incoming download
			hook_CL_NextDownload.create(0x00410190, stub_CL_NextDownload);
		}

		void post_cgame() override
		{
			// Use a cvar whitelist for setClientCvar GSC method
			hook_CG_ServerCommand.create(ABSOLUTE_CGAME_MP(0x3002e0d0), stub_CG_ServerCommand);
		}
	};
}

REGISTER_COMPONENT(security::component)
#endif