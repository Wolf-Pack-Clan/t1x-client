#include <std_include.hpp>
#if 1
#include <hook.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include <string.hpp>

#include "imgui.hpp"
#include "movement.hpp"

#include <hidusage.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace window
{
	int rawinput_x_current = 0;
	int rawinput_y_current = 0;
	int rawinput_x_old = 0;
	int rawinput_y_old = 0;

	HHOOK hHook;

	game::cvar_t* r_fullscreen;
	game::cvar_t* cl_bypassMouseInput;

	utils::hook::detour Com_Init_hook;
	utils::hook::detour IN_MouseMove_hook;

	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (GetForegroundWindow() == *game::hWnd)
		{
			if (nCode == HC_ACTION)
			{
				auto pKey = (KBDLLHOOKSTRUCT*)lParam;

				auto altDown = (pKey->flags & LLKHF_ALTDOWN);
				auto escDown_sys = (pKey->vkCode == VK_ESCAPE && wParam == WM_SYSKEYDOWN);
				auto escUp_sys = (pKey->vkCode == VK_ESCAPE && wParam == WM_SYSKEYUP);

				if (altDown)
				{
					if (escDown_sys)
					{
						if (!imgui::waitForMenuKeyRelease)
						{
							imgui::toggle_menu_flag();
							imgui::waitForMenuKeyRelease = true;
						}
						return 1; // Prevent Windows Alt+Esc behavior
					}
					else if (escUp_sys)
					{
						imgui::waitForMenuKeyRelease = false;
					}
				}
				else if (imgui::waitForMenuKeyRelease)
				{
					// Released Alt before releasing Esc
					imgui::waitForMenuKeyRelease = false;
				}
			}
		}

		return CallNextHookEx(hHook, nCode, wParam, lParam);
	}
	
	static void rawInput_init(HWND hWnd)
	{
		RAWINPUTDEVICE rid[1]{};
		rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		rid[0].dwFlags = RIDEV_INPUTSINK;
		rid[0].hwndTarget = hWnd;
		if (!RegisterRawInputDevices(rid, ARRAYSIZE(rid), sizeof(rid[0])))
			throw std::runtime_error("RegisterRawInputDevices failed");
	}
	
	uintptr_t CL_MouseEvent_addr = 0x0040b0a0;
	static void CL_MouseEvent(int _dx, int _dy)
	{
		_asm
		{
			mov ecx, _dx
			push eax
			mov eax, _dy
			call CL_MouseEvent_addr
			add esp, 4
		}
	}
	
	static void rawInput_move()
	{
		auto delta_x = rawinput_x_current - rawinput_x_old;
		auto delta_y = rawinput_y_current - rawinput_y_old;

		rawinput_x_old = rawinput_x_current;
		rawinput_y_old = rawinput_y_current;
		
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		CL_MouseEvent(delta_x, delta_y);
	}
	
	static void IN_MouseMove_stub()
	{
		// Apply raw input only when player can move
		if (movement::m_rawinput->integer)
		{
			if (*game::cls_keyCatchers == 0) // TODO: Figure out why have to use "== 0" instead of "& KEYCATCH_CGAME"
			{
				rawInput_move();
				return;
			}

			// If a .menu is displayed, and cl_bypassMouseInput is enabled, player can move (e.g. wm_quickmessage.menu)
			if ((*game::cls_keyCatchers & KEYCATCH_UI) && cl_bypassMouseInput->integer)
			{
				rawInput_move();
				return;
			}
			
			if (r_fullscreen->integer && *game::cgvm != NULL)
			{
				// .menu + console opened = player can't move
				if (*game::cls_keyCatchers == 3)
				{
					IN_MouseMove_hook.invoke();
					return;
				}

				if (*game::cls_keyCatchers & KEYCATCH_CONSOLE)
				{
					rawInput_move();
					return;
				}
			}

			if (*game::cls_keyCatchers & KEYCATCH_MESSAGE)
			{
				rawInput_move();
				return;
			}
		}

		IN_MouseMove_hook.invoke();
	}
	
	static void WM_INPUT_process(LPARAM lParam)
	{
		//// Don't update raw input when:
		if (imgui::displayed)
			return;
		// a .menu is displayed (except if it uses cl_bypassMouseInput)
		if (*game::cls_keyCatchers & KEYCATCH_UI && !cl_bypassMouseInput->integer)
			return;
		// console is opened and game is windowed
		if (*game::cls_keyCatchers & KEYCATCH_CONSOLE && !r_fullscreen->integer)
			return;
		// using another window
		if (GetForegroundWindow() != *game::hWnd)
			return;
		////
		
		UINT dwSize = sizeof(RAWINPUT);
		static RAWINPUT raw;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));
		rawinput_x_current += raw.data.mouse.lLastX;
		rawinput_y_current += raw.data.mouse.lLastY;
	}
	
	static LRESULT CALLBACK MainWndProc_stub(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (imgui::displayed && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;
		
		switch (uMsg)
		{
		case WM_INPUT:
			WM_INPUT_process(lParam);
			return true;
		case WM_CREATE:
			SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_MINIMIZEBOX); // Add minimize button and Win+M support
			rawInput_init(hWnd);
			break;
		case WM_CHAR:
			if (wParam == VK_ESCAPE && imgui::displayed)
				imgui::toggle_menu_flag(); // Allow closing imgui by pressing only Esc
			break;
		case WM_KEYDOWN:
			if (imgui::displayed)
				return 0; // Prevent moving
			break;
		case WM_MOUSEWHEEL:
			if (imgui::displayed)
				return 0; // Prevent changing weapon
			break;
		case WM_MENUCHAR:
			return MNC_CLOSE << 16; // Prevent Alt+Enter beep sound
		}

		// See https://github.com/kartjom/CoDPlusPlus/blob/359539f889958b2cbd58884cbc5bb0e3e5a3c294/CoDPlusPlus/src/Utils/WinApiHelper.cpp#L210
		if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
		{
			/*
			When opening imgui, it can fail getting focus if Alt+Esc was not pressed in a (non-natural) particular way
			Returning here prevents this
			*/
			if (imgui::displayed)
				return 0;

			/*
			When closing imgui, if game is windowed and console is open,
			console's text field might lose focus because of the Alt press, and the (non visible) system menu woult obtain it
			Returning here prevents this
			*/
			if (*game::cls_keyCatchers & KEYCATCH_CONSOLE)
				return 0;
		}
		
		return game::MainWndProc(hWnd, uMsg, wParam, lParam);
	}
	
	static void Com_Init_stub(char* commandLine)
	{
		hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
		if (!hHook)
			throw std::runtime_error(utils::string::va("SetWindowsHookEx for LowLevelKeyboardProc failed"));
		
		Com_Init_hook.invoke(commandLine);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			r_fullscreen = game::Cvar_Get("r_fullscreen", "0", CVAR_ARCHIVE | CVAR_LATCH);
			cl_bypassMouseInput = game::Cvar_Get("cl_bypassMouseInput", "0", 0);

			utils::hook::set(0x4639b9 + 1, MainWndProc_stub);
			utils::hook::set(0x5083b1, 0x00); // Alt+Tab support, see https://github.com/xtnded/codextended-client/pull/1
			
			Com_Init_hook.create(0x004375c0, Com_Init_stub);
			IN_MouseMove_hook.create(0x00461850, IN_MouseMove_stub);
		}
	};
}

REGISTER_COMPONENT(window::component)
#endif