#include "_string.h"
#include "loader/component_loader.h"
#include "security.h"
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
	void end_frame();
}