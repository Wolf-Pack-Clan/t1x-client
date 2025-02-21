#include "hook.h"
#include "_string.h"

#include "loader/component_loader.h"

#include "imgui.h"
#include "movement.h"
#include "scheduler.h"

#include <hidusage.h>

namespace window
{
	void MSG(const std::string& text, UINT flags = MB_OK);
}