#include "pch.h"
#if 1
#include "discord.h"

namespace discord
{
	DiscordRichPresence presence{};
	std::time_t init_timestamp = -1;
	stock::cvar_t* discord;

#ifdef RELEASE
#pragma warning(push)
#pragma warning(disable: 4100)
#endif
	void ready(const DiscordUser* request)
	{
#ifdef RELEASE
#pragma warning(pop)
#endif
#ifdef DEBUG
		std::stringstream ss;
		ss << "####### discord ready" << std::endl;
		OutputDebugString(ss.str().c_str());
#endif
		updateInfo();
	}

#ifdef DEBUG
	void errored(const int error_code, const char* message)
	{
		std::stringstream ss;
		ss << "####### discord errored, error_code: " << error_code << ", message: " << message << std::endl;
		OutputDebugString(ss.str().c_str());
	}
#endif








	/*void join_game(const char* join_secret)
	{
		

	}
	void join_request(const DiscordUser* request)
	{

	}*/





	
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

		if (init_timestamp == -1)
		{
			init_timestamp = std::time(nullptr);
			presence.startTimestamp = init_timestamp;
			Discord_UpdatePresence(&presence);
		}
		
		if (*stock::cls_state == stock::CA_ACTIVE)
		{
			char* cl_gameState_stringData = (char*)0x01436a7c;
			int* cl_gameState_stringOffsets = (int*)0x1434A7C;
			char* serverInfo = cl_gameState_stringData + cl_gameState_stringOffsets[stock::CS_SERVERINFO];
			
			std::string g_gametype = stock::Info_ValueForKey(serverInfo, "g_gametype");
			std::string mapname = stock::Info_ValueForKey(serverInfo, "mapname");
			std::string sv_maxclients = stock::Info_ValueForKey(serverInfo, "sv_maxclients");
			std::string sv_hostname = stock::Info_ValueForKey(serverInfo, "sv_hostname");
			sv_hostname = utils::string::clean(sv_hostname, true);
			
			std::string g_gametype_mapname = g_gametype + " - " + mapname;
			// TODO: Access connected players count
			std::string connectedClients_sv_maxclients_sv_hostname = std::string("(?/") + sv_maxclients + ") - " + sv_hostname;

			presence.details = g_gametype_mapname.c_str();
			presence.state = connectedClients_sv_maxclients_sv_hostname.c_str();
			
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
			/*handlers.joinGame = join_game;
			handlers.joinRequest = join_request;*/
			
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