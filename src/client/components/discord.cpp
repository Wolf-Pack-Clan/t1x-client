#include "pch.h"
#if 1
#include "discord.h"

namespace discord
{
	DiscordRichPresence presence{};
	std::time_t init_timestamp = -1;
	stock::cvar_t* discord;
	bool isReady = false;

	static void ready(const DiscordUser*)
	{
#ifdef DEBUG
		std::stringstream ss;
		ss << "####### discord ready" << std::endl;
		OutputDebugString(ss.str().c_str());
#endif
		isReady = true;
		updateInfo();
	}

#ifdef DEBUG
	static void errored(const int error_code, const char* message)
	{
		std::stringstream ss;
		ss << "####### discord errored, error_code: " << error_code << ", message: " << message << std::endl;
		OutputDebugString(ss.str().c_str());
	}
#endif
	
	static void disconnected(const int, const char*)
	{
#ifdef DEBUG
		std::stringstream ss;
		ss << "####### discord disconnected" << std::endl;
		OutputDebugString(ss.str().c_str());
#endif
		isReady = false;
		init_timestamp = -1;
	}
	
	void updateInfo()
	{
		if (!discord->integer)
		{
			if (init_timestamp != -1)
			{
				init_timestamp = -1;
				Discord_ClearPresence();
			}
			return;
		}

		if (init_timestamp == -1 && isReady)
		{
			init_timestamp = std::time(nullptr);
			presence.startTimestamp = init_timestamp;
			Discord_UpdatePresence(&presence);
		}
		
		if (*stock::cls_state == stock::CA_ACTIVE && !*stock::clc_demoplaying)
		{
			char* cl_gameState_stringData = (char*)0x01436a7c;
			int* cl_gameState_stringOffsets = (int*)0x1434A7C;
			char* serverInfo = cl_gameState_stringData + cl_gameState_stringOffsets[stock::CS_SERVERINFO];
			
			std::string g_gametype = stock::Info_ValueForKey(serverInfo, "g_gametype");
			std::string mapname = stock::Info_ValueForKey(serverInfo, "mapname");
			std::string sv_maxclients = stock::Info_ValueForKey(serverInfo, "sv_maxclients");
			std::string sv_hostname = stock::Info_ValueForKey(serverInfo, "sv_hostname");
			sv_hostname = utils::string::clean(sv_hostname, true);

			/*
			Both seem to provide the count of connected players
			int numPlayers = *(int*)(address_cgame_mp + 0x1f6a34);
			TODO: Clean
			*/
			int numPlayers = *(int*)(address_cgame_mp + 0x1e4248);
			
			presence.details = sv_hostname.c_str();
			std::string state = mapname + " | " + std::to_string(numPlayers) + " (" + sv_maxclients + ")" + " | " + g_gametype;
			presence.state = state.c_str();





			presence.buttonLabel[0] = "test1";
			presence.buttonUrl[0] = "https://google.com";


			presence.buttonLabel[1] = "test2";
			presence.buttonUrl[1] = "https://youtube.com";






			Discord_UpdatePresence(&presence);

		}
		else
		{
			if (presence.details != nullptr || presence.state != nullptr)
			{
				presence = {};
				presence.startTimestamp = init_timestamp;
				Discord_UpdatePresence(&presence);
			}
		}
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			discord = stock::Cvar_Get("discord", "0", stock::CVAR_ARCHIVE);
			
			DiscordEventHandlers handlers{};
			handlers.ready = ready;
#ifdef DEBUG
			handlers.errored = errored;
#endif
			handlers.disconnected = disconnected;
			
			Discord_Initialize("1343751922112532480", &handlers, 1, nullptr);
			this->initialized = true;
			
			scheduler::loop(Discord_RunCallbacks, scheduler::async, 1s);
			scheduler::loop(updateInfo, scheduler::async, 2s);
		}

		void pre_destroy() override
		{
			if (this->initialized)
				Discord_Shutdown();
		}

	private:
		bool initialized = false;
	};
}

REGISTER_COMPONENT(discord::component)
#endif