#pragma once

#include "shared.h"

#include "hook.h"
#include "_string.h"

#include "loader/component_loader.h"

#include "imgui.h"
#include "movement.h"
#include "scheduler.h"
#include "security.h"

#include <hidusage.h>

namespace window
{
	extern char sys_cmdline[stock::MAX_STRING_CHARS];

	void MSG(const std::string& text, UINT flags = MB_OK);
}