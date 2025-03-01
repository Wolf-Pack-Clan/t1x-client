#pragma once

#include "shared.h"

#include "hook.h"
#include "_string.h"

#include "loader/component_loader.h"

#include "scheduler.h"
#include "window.h"

#include "discord_rpc.h"

namespace discord
{
	extern stock::cvar_t* discord;;

	void updateInfo();
}