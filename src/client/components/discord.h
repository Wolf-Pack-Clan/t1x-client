#pragma once

#include "shared.h"

#include "hook.h"
#include "_string.h"

#include "loader/component_loader.h"

#include "scheduler.h"

#include "discord_rpc.h"

namespace discord
{
	extern stock::cvar_t* discord;;

	void ready(const DiscordUser* request);
	void errored(const int error_code, const char* message);
	void updateInfo();
}