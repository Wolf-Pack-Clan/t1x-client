#include "pch.h"
#if 1
#include "discord.h"

namespace discord
{
	bool isReady = false;
	bool updatesStarted = false;
	bool exiting = false;
	std::time_t timestamp_init = -1;
	std::thread thread_checkForJoinRequest;
	HANDLE hPipe;
	stock::cvar_t* discord;
	DiscordRichPresence presence{};

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
	}
	
	static void checkForJoinRequest()
	{
		while(true)
		{
			BOOL connected = ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED;

			if (exiting)
				return;

			if (connected)
			{
				//MessageBox(NULL, "connected", "", NULL);
				DWORD bytesRead;
				char buffer[512]{};
				BOOL success = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
				if (success)
				{
					//MessageBox(NULL, "success", "", NULL);
					buffer[bytesRead] = '\0';
					std::string command = std::string("connect ") + buffer + "\n";
					stock::Cbuf_ExecuteText(stock::EXEC_APPEND, command.c_str());
				}
				else
					MessageBox(NULL, "Failed to read from pipe for Discord", "", NULL);
				DisconnectNamedPipe(hPipe);
			}
			else
				MessageBox(NULL, "Failed to connect pipe for Discord", "", NULL);
		}
	}
	
	void updateInfo()
	{
		if (!discord->integer)
		{
			if (updatesStarted)
			{
				// Was enabled
				Discord_ClearPresence();
				updatesStarted = false;
			}
			return;
		}
		else
		{
			if (!isReady)
				return;
			
			if (!updatesStarted)
			{
				// First update
				Discord_UpdatePresence(&presence);
				updatesStarted = true;
			}
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
			
			presence.buttonLabel[0] = "Join";
			std::string url = std::string("iw1x://") + stock::NET_AdrToString(*stock::clc_serverAddress);
			presence.buttonUrl[0] = url.c_str();
			
			Discord_UpdatePresence(&presence);
		}
		else
		{
			if (presence.details != nullptr || presence.state != nullptr)
			{
				// Was in a server, reset info
				presence = {};
				presence.startTimestamp = timestamp_init;
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
			
			timestamp_init = std::time(nullptr);
			presence.startTimestamp = timestamp_init;
			
			scheduler::loop(Discord_RunCallbacks, scheduler::client);
			scheduler::loop(updateInfo, scheduler::client);
			
			hPipe = CreateNamedPipe(
				"\\\\.\\pipe\\iw1x",
				PIPE_ACCESS_INBOUND,
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
				1, // nMaxInstances
				512, 512, 0, NULL
			);
			if (hPipe == INVALID_HANDLE_VALUE)
				window::MSG("Failed to create pipe for Discord", MB_ICONERROR | MB_SETFOREGROUND);
			else
				thread_checkForJoinRequest = utils::thread::create_named_thread("checkForJoinRequest", checkForJoinRequest);
		}

		void pre_destroy() override
		{
			if (this->initialized)
				Discord_Shutdown();
			
			exiting = true;
			CancelIoEx(hPipe, NULL);
			CloseHandle(hPipe);
			if (thread_checkForJoinRequest.joinable())
				thread_checkForJoinRequest.join();
		}

	private:
		bool initialized = false;
	};
}

REGISTER_COMPONENT(discord::component)
#endif