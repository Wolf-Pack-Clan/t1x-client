#pragma once

namespace imgui
{
	extern bool displayed;
	extern bool waitForMenuKeyRelease;

	void menu_updates_settings();
	void toggle_menu_flag();
	void new_frame();
	void draw_menu();
	void end_frame();
}