#include <std_include.hpp>
#if 1
#include <hook.hpp>
#include "loader/component_loader.hpp"
#include "stock/game.hpp"

#include "security.hpp"

namespace security
{
	game::cvar_t* cl_allowDownload;

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
		auto cmd = game::Cmd_Argv(0);
		if (*cmd == 'v')
		{
			auto cvar_name = game::Cmd_Argv(1);
#if 0
			std::ostringstream oss;
			oss << "####### CG_ServerCommand_stub: " << cvar_name << "\n";
			std::string str = oss.str();
			OutputDebugString(str.c_str());
#endif

			if (!cvarIsInWhitelist(cvar_name))
				return;
		}
		CG_ServerCommand_hook.invoke();
	}

	static void CL_SystemInfoChanged_Cvar_Set_stub(const char* name, const char* value)
	{
#if 0
		std::ostringstream oss;
		oss << "####### CL_SystemInfoChanged_Cvar_Set_stub: " << name << "\n";
		std::string str = oss.str();
		OutputDebugString(str.c_str());
#endif

		if (!cvarIsInWhitelist(name))
			return;
		game::Cvar_Set(name, value);
	}

	static void CL_SystemInfoChanged_stub()
	{
		char* cl_gameState_stringData = (char*)0x01436a7c;
		int* cl_gameState_stringOffsets = (int*)0x01434a80;
		char* systemInfo = cl_gameState_stringData + cl_gameState_stringOffsets[0];
		const char* sv_pakNames = game::Info_ValueForKey(systemInfo, "sv_pakNames");
		const char* sv_referencedPakNames = game::Info_ValueForKey(systemInfo, "sv_referencedPakNames");

		if (strstr(sv_pakNames, "@") || strstr(sv_referencedPakNames, "@"))
			game::Com_Error(game::ERR_DROP, "Non-pk3 download protection triggered");

		CL_SystemInfoChanged_hook.invoke();
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

			cl_allowDownload = game::Cvar_Get("cl_allowDownload", "0", CVAR_ARCHIVE);
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