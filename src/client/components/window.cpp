#include "pch.h"
#if 1
#include "window.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace window
{
	int rawinput_x_current = 0;
	int rawinput_y_current = 0;
	int rawinput_x_old = 0;
	int rawinput_y_old = 0;
	char sys_cmdline[stock::MAX_STRING_CHARS];
	HHOOK hHook;

	utils::hook::detour hook_Com_Init;
	utils::hook::detour hook_IN_MouseMove;
	utils::hook::detour hook_SV_Startup;
	utils::hook::detour hook_SV_Shutdown;

	void MSG(const std::string& text, UINT flags)
	{
		scheduler::once([text, flags]() { MessageBox(*stock::hWnd, text.c_str(), MOD_NAME, flags); }, scheduler::async);
	}

	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (GetForegroundWindow() == *stock::hWnd)
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
							imgui::toggle_menu(false);
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
	
	static void rawInput_move()
	{
		auto delta_x = rawinput_x_current - rawinput_x_old;
		auto delta_y = rawinput_y_current - rawinput_y_old;

		rawinput_x_old = rawinput_x_current;
		rawinput_y_old = rawinput_y_current;
		
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		stock::CL_MouseEvent(delta_x, delta_y);
	}
	
	static void stub_IN_MouseMove()
	{
		// Apply raw input only when player can move // TODO: Maybe hook CG_MouseEvent instead then
		if (movement::m_rawinput->integer)
		{
			if (*stock::cls_keyCatchers == 0) // TODO: Figure out why have to use "== 0" instead of "& KEYCATCH_CGAME"
			{
				rawInput_move();
				return;
			}

			// If a .menu is displayed, and cl_bypassMouseInput is enabled, player can move (e.g. wm_quickmessage.menu)
			if ((*stock::cls_keyCatchers & stock::KEYCATCH_UI) && cvars::cl_bypassMouseInput->integer)
			{
				rawInput_move();
				return;
			}
			
			if (cvars::r_fullscreen->integer && *stock::cgvm != NULL)
			{
				// .menu + console opened = player can't move
				if (*stock::cls_keyCatchers == 3)
				{
					hook_IN_MouseMove.invoke();
					return;
				}

				if (*stock::cls_keyCatchers & stock::KEYCATCH_CONSOLE)
				{
					rawInput_move();
					return;
				}
			}

			if (*stock::cls_keyCatchers & stock::KEYCATCH_MESSAGE)
			{
				rawInput_move();
				return;
			}
		}

		hook_IN_MouseMove.invoke();
	}
	
	static void WM_INPUT_process(LPARAM lParam)
	{
		//// Don't update raw input when:
		if (imgui::displayed)
			return;
		// a .menu is displayed (except if it uses cl_bypassMouseInput)
		if (*stock::cls_keyCatchers & stock::KEYCATCH_UI && !cvars::cl_bypassMouseInput->integer)
			return;
		// console is opened and game is windowed
		if (*stock::cls_keyCatchers & stock::KEYCATCH_CONSOLE && !cvars::r_fullscreen->integer)
			return;
		// using another window
		if (GetForegroundWindow() != *stock::hWnd)
			return;
		////
		
		UINT dwSize = sizeof(RAWINPUT);
		static RAWINPUT raw;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));
		rawinput_x_current += raw.data.mouse.lLastX;
		rawinput_y_current += raw.data.mouse.lLastY;
	}
	
	static LRESULT CALLBACK stub_MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (imgui::displayed && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;
		
		switch (uMsg)
		{
		case WM_INPUT:
			WM_INPUT_process(lParam);
			return true;
		case WM_CREATE:
			SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
			rawInput_init(hWnd);
			break;
		case WM_CHAR:
			if (wParam == VK_ESCAPE && imgui::displayed)
				imgui::toggle_menu(false);
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				if (security::escape_aborted_connection())
					return 0;
			
			if (imgui::displayed)
				return 0; // Prevent moving
			break;
		case WM_MOUSEWHEEL:
			if (imgui::displayed)
				return 0; // Prevent changing weapon
			break;
		case WM_MENUCHAR:
			return MNC_CLOSE << 16; // Prevent Alt+Enter beep sound
		case WM_SYSCOMMAND:
			if (wParam == SC_MAXIMIZE)
			{
				stock::Cvar_Set(cvars::r_fullscreen->name, "1");
				stock::Cbuf_ExecuteText(stock::EXEC_APPEND, "vid_restart\n");
				return 0;
			}
			break;
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
			if (*stock::cls_keyCatchers & stock::KEYCATCH_CONSOLE)
				return 0;
		}
		
		return stock::MainWndProc(hWnd, uMsg, wParam, lParam);
	}
	
	static void stub_Com_Init(char*)
	{
		hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
		if (!hHook)
			throw std::runtime_error(utils::string::va("SetWindowsHookEx for LowLevelKeyboardProc failed"));
		
		hook_Com_Init.invoke(sys_cmdline);
	}
	
	static void stub_SV_Startup()
	{
		auto style = GetWindowLong(*stock::hWnd, GWL_STYLE);
		style &= ~WS_MAXIMIZEBOX;
		SetWindowLong(*stock::hWnd, GWL_STYLE, style);

		hook_SV_Startup.invoke();
	}

	static void stub_SV_Shutdown(char *finalmsg)
	{
		hook_SV_Shutdown.invoke(finalmsg);

		auto style = GetWindowLong(*stock::hWnd, GWL_STYLE);
		style |= WS_MAXIMIZEBOX;
		SetWindowLong(*stock::hWnd, GWL_STYLE, style);
	}

	static void Cmd_Minimize()
	{
		ShowWindow(*stock::hWnd, SW_MINIMIZE);
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			stock::Cmd_AddCommand("minimize", Cmd_Minimize);

			utils::hook::set(0x4639b9 + 1, stub_MainWndProc);
			utils::hook::set(0x5083b1, 0x00); // Alt+Tab support, see https://github.com/xtnded/codextended-client/pull/1
			
			hook_Com_Init.create(0x004375c0, stub_Com_Init);
			hook_IN_MouseMove.create(0x00461850, stub_IN_MouseMove);
			hook_SV_Startup.create(0x00458160, stub_SV_Startup);
			hook_SV_Shutdown.create(0x00459600, stub_SV_Shutdown);
		}
	};
}

REGISTER_COMPONENT(window::component)
#endif