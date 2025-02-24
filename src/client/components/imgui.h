#pragma once

#include "shared.h"

#include "_string.h"

#include "loader/component_loader.h"

#include "ui.h"
#include "movement.h"
#include "view.h"

namespace imgui
{
	extern bool displayed;
	extern bool waitForMenuKeyRelease;

	void menu_updates_settings();
	void toggle_menu_flag();
	void new_frame();
	void draw_menu();
	void draw_menu_tab_Security();
	void draw_menu_tab_UI();
	void draw_menu_tab_View();
	void draw_menu_tab_Movement();
	void end_frame();
}