#pragma once

namespace security
{
	void ready_hook_ui_mp();
	void ready_hook_cgame_mp();

	extern game::cvar_t* cl_allowDownload;
}