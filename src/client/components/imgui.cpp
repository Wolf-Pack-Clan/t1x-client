#include "pch.h"
#if 1
#include "imgui.h"

namespace imgui
{
#define BEGINTABITEM_SPACE(label) if (ImGui::BeginTabItem(label)) { ImGui::Dummy(ImVec2(0, 5));
#define ENDTABITEM_SPACED() ImGui::EndTabItem(); }

	bool initialized = false;
	bool displayed = false;
	bool waitForMenuKeyRelease = false;
	float indentation_checkbox_child = 12.f;

	HGLRC imguiWglContext;
	HWND hWnd_during_init;

	bool sensitivity_adsScaleEnable = false;
	float sensitivity_adsScale = 0.f;
	bool sensitivity_adsScaleSniperEnable = false;
	float sensitivity_adsScaleSniper = 0.f;
	bool cg_drawDisconnect = false;
	bool cg_drawWeaponSelect = false;
	bool cg_drawFPS = false;
	bool cg_drawFPS_custom = false;
	int cg_chatHeight = 0;
	int con_boldgamemessagetime = 0;
	bool cg_lagometer = false;
	bool cl_allowDownload = false;
	bool m_rawinput = false;
	float cg_fov = 0.f;
	bool cg_fovScaleEnable = false;
	float cg_fovScale = 0.f;
	bool record_respawn = false;
	float com_timescale = 0.f;
	bool discord = false;	
	bool branding = false;
	bool cg_drawPing = false;

	static void _toggle_menu()
	{
		toggle_menu(false);
	}

	void toggle_menu(bool closedUsingButton)
	{
		if (closedUsingButton)
		{
			*stock::mouseInitialized = stock::qtrue;
			stock::IN_ActivateMouse();
			return;
		}
		
		if (!displayed)
		{
			displayed = true;
			stock::IN_DeactivateMouse();
			*stock::mouseActive = stock::qfalse;
			*stock::mouseInitialized = stock::qfalse;
		}
		else
		{
			displayed = false;
			*stock::mouseInitialized = stock::qtrue;
			stock::IN_ActivateMouse();
		}
	}

	static void init(HDC hdc)
	{
		hWnd_during_init = *stock::hWnd;
		imguiWglContext = wglCreateContext(hdc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = "iw1x_imgui.ini";
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 16.0f);

		ImGui_ImplWin32_InitForOpenGL(*stock::hWnd);
		ImGui_ImplOpenGL2_Init();

		initialized = true;
	}

	static void gui_on_frame()
	{
		new_frame();
		draw_menu();
		end_frame();
	}
	
	void new_frame()
	{
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	
	static void menu_loads_settings()
	{
		sensitivity_adsScaleEnable = movement::sensitivity_adsScaleEnable->integer;
		sensitivity_adsScale = movement::sensitivity_adsScale->value;
		sensitivity_adsScaleSniperEnable = movement::sensitivity_adsScaleSniperEnable->value;
		sensitivity_adsScaleSniper = movement::sensitivity_adsScaleSniper->value;
		cg_drawDisconnect = ui::cg_drawDisconnect->integer;
		cg_drawWeaponSelect = ui::cg_drawWeaponSelect->integer;
		con_boldgamemessagetime = cvars::con_boldgamemessagetime->integer;
		cl_allowDownload = !cvars::cl_allowDownload->integer;
		m_rawinput = movement::m_rawinput->integer;
		cg_fovScaleEnable = view::cg_fovScaleEnable->integer;
		cg_fovScale = view::cg_fovScale->value;
		record_respawn = view::record_respawn->integer;
		com_timescale = cvars::com_timescale->value;
		discord = discord::discord->integer;
		branding = ui::branding->integer;
		cg_drawFPS_custom = ui::cg_drawFPS_custom->integer;
		cg_drawPing = ui::cg_drawPing->integer;

		if (*stock::cgvm != NULL)
		{
			cg_lagometer = cvars::vm::cg_lagometer->integer;
			cg_drawFPS = cvars::vm::cg_drawFPS->integer;
			cg_chatHeight = cvars::vm::cg_chatHeight->integer;
			cg_fov = cvars::vm::cg_fov->value;
		}
	}
	
	void draw_menu()
	{
		menu_loads_settings();

		ImGui::SetNextWindowSize(ImVec2(300, 0));
		ImGui::SetNextWindowPos(ImVec2(25, 80), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowFocus();
		ImGui::Begin("##", &displayed, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		if (ImGui::BeginTabBar("TabBar"))
		{
			draw_menu_tab_Security();
			draw_menu_tab_UI();
			draw_menu_tab_View();
			draw_menu_tab_Movement();
			draw_menu_tab_Social();

			ImGui::EndTabBar();
		}
		
		ImGui::End();
		
		if (!displayed)
			toggle_menu(true);
		
		menu_updates_settings();
	}

	void draw_menu_tab_Security()
	{
		BEGINTABITEM_SPACE("Security")
			
			ImGui::Checkbox("Prevent downloading", &cl_allowDownload);

		ENDTABITEM_SPACED()
	}

	void draw_menu_tab_UI()
	{
		BEGINTABITEM_SPACE("UI")

			if (*stock::cgvm == NULL) ImGui::BeginDisabled();
			ImGui::Checkbox("FPS counter", &cg_drawFPS);
			if (*stock::cgvm == NULL) ImGui::EndDisabled();
			ImGui::Indent(indentation_checkbox_child);
			ImGui::Checkbox("Custom", &cg_drawFPS_custom);
			ImGui::Unindent(indentation_checkbox_child);

			ImGui::Spacing();

			if (*stock::clc_demoplaying) ImGui::BeginDisabled();
			ImGui::Checkbox("Ping", &cg_drawPing);
			if (*stock::clc_demoplaying) ImGui::EndDisabled();
			if (*stock::cgvm == NULL || cvars::com_sv_running->integer) ImGui::BeginDisabled();
			ImGui::Checkbox("Lagometer", &cg_lagometer);
			if (*stock::cgvm == NULL || cvars::com_sv_running->integer) ImGui::EndDisabled();

			ImGui::Checkbox("Connection interrupted indicator", &cg_drawDisconnect);
			ImGui::Checkbox("Weapon selection", &cg_drawWeaponSelect);

			ImGui::Spacing();

			if (*stock::cgvm == NULL) ImGui::BeginDisabled();
			ImGui::Text("Chat lines");
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::SliderInt("##slider_cg_chatHeight", &cg_chatHeight, 0, 8, "%i", ImGuiSliderFlags_NoInput);
			if (*stock::cgvm == NULL) ImGui::EndDisabled();

			ImGui::Spacing();

			ImGui::Text("Middle messages seconds");
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::SliderInt("##slider_con_boldgamemessagetime", &con_boldgamemessagetime, 0, 8, "%i", ImGuiSliderFlags_NoInput);

			ImGui::Spacing();

			ImGui::Checkbox("Branding", &branding);

		ENDTABITEM_SPACED()
	}

	void draw_menu_tab_View()
	{
		BEGINTABITEM_SPACE("View")

			ImGui::Checkbox("Record when respawn", &record_respawn);

			ImGui::Spacing();

			if (*stock::cgvm == NULL) ImGui::BeginDisabled();
			ImGui::Text("FOV");
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::SliderFloat("##slider_cg_fov", &cg_fov, 80.f, 95.f, "%.2f", ImGuiSliderFlags_NoInput);
			if (*stock::cgvm == NULL) ImGui::EndDisabled();

			ImGui::Spacing();

			ImGui::Checkbox("FOV scale", &cg_fovScaleEnable);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (!cg_fovScaleEnable) ImGui::BeginDisabled();
			ImGui::SliderFloat("##slider_cg_fovScale", &cg_fovScale, 1.f, 1.4f, "%.2f", ImGuiSliderFlags_NoInput);
			if (!cg_fovScaleEnable) ImGui::EndDisabled();

			ImGui::Spacing();
			
			if (*stock::cgvm == NULL || !cvars::sv_cheats->integer) ImGui::BeginDisabled();
			ImGui::Text("Time scale");
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::SliderFloat("##slider_com_timescale", &com_timescale, 0.1f, 5, "%.1f", ImGuiSliderFlags_NoInput);
			if (*stock::cgvm == NULL || !cvars::sv_cheats->integer) ImGui::EndDisabled();

		ENDTABITEM_SPACED()
	}

	void draw_menu_tab_Movement()
	{
		BEGINTABITEM_SPACE("Movement")

			ImGui::Checkbox("Raw mouse input", &m_rawinput);

			ImGui::Spacing();

			// ADS sensitivity scale
			ImGui::Checkbox("ADS sensitivity scale", &sensitivity_adsScaleEnable);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (!sensitivity_adsScaleEnable) ImGui::BeginDisabled();
			ImGui::SliderFloat("##slider_sensitivity_adsScale", &sensitivity_adsScale, 0.15f, 1.f, "%.2f", ImGuiSliderFlags_NoInput);
			if (!sensitivity_adsScaleEnable) ImGui::EndDisabled();

			ImGui::Spacing();

			// ADS sensitivity scale sniper
			ImGui::Checkbox("ADS sensitivity scale [sniper]", &sensitivity_adsScaleSniperEnable);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (!sensitivity_adsScaleSniperEnable) ImGui::BeginDisabled();
			ImGui::SliderFloat("##slider_sensitivity_adsScaleSniper", &sensitivity_adsScaleSniper, 0.15f, 1.f, "%.2f", ImGuiSliderFlags_NoInput);
			if (!sensitivity_adsScaleSniperEnable) ImGui::EndDisabled();

		ENDTABITEM_SPACED()
	}

	void draw_menu_tab_Social()
	{
		BEGINTABITEM_SPACE("Social")
			
			ImGui::Checkbox("Discord Activity Status", &discord);

		ENDTABITEM_SPACED()
	}

	void menu_updates_settings()
	{
		stock::Cvar_Set(movement::sensitivity_adsScaleEnable->name, sensitivity_adsScaleEnable ? "1" : "0");
		stock::Cvar_Set(movement::sensitivity_adsScale->name, utils::string::va("%.2f", sensitivity_adsScale));
		stock::Cvar_Set(movement::sensitivity_adsScaleSniperEnable->name, sensitivity_adsScaleSniperEnable ? "1" : "0");
		stock::Cvar_Set(movement::sensitivity_adsScaleSniper->name, utils::string::va("%.2f", sensitivity_adsScaleSniper));
		stock::Cvar_Set(ui::cg_drawDisconnect->name, cg_drawDisconnect ? "1" : "0");
		stock::Cvar_Set(ui::cg_drawWeaponSelect->name, cg_drawWeaponSelect ? "1" : "0");
		stock::Cvar_Set(cvars::con_boldgamemessagetime->name, utils::string::va("%i", con_boldgamemessagetime));
		stock::Cvar_Set(cvars::cl_allowDownload->name, cl_allowDownload ? "0" : "1");
		stock::Cvar_Set(movement::m_rawinput->name, m_rawinput ? "1" : "0");
		stock::Cvar_Set(view::cg_fovScaleEnable->name, cg_fovScaleEnable ? "1" : "0");
		stock::Cvar_Set(view::cg_fovScale->name, utils::string::va("%.2f", cg_fovScale));
		stock::Cvar_Set(view::record_respawn->name, record_respawn ? "1" : "0");
		stock::Cvar_Set(cvars::com_timescale->name, utils::string::va("%.1f", com_timescale));
		stock::Cvar_Set(discord::discord->name, discord ? "1" : "0");
		stock::Cvar_Set(ui::cg_drawFPS_custom->name, cg_drawFPS_custom ? "1" : "0");
		stock::Cvar_Set(ui::branding->name, branding ? "1" : "0");
		stock::Cvar_Set(ui::cg_drawPing->name, cg_drawPing ? "1" : "0");
		
		if (*stock::cgvm != NULL)
		{
			stock::cvar_t* cv = NULL;
			/*
			TODO: Clean, see Cvar_Update
			*/
			cv = reinterpret_cast<stock::cvar_t*>(reinterpret_cast<uintptr_t>(*stock::cvar_indexes) + (0x2c * cvars::vm::cg_fov->handle));
			stock::Cvar_Set(cv->name, utils::string::va("%.2f", cg_fov));
			
			cv = reinterpret_cast<stock::cvar_t*>(reinterpret_cast<uintptr_t>(*stock::cvar_indexes) + (0x2c * cvars::vm::cg_drawFPS->handle));
			stock::Cvar_Set(cv->name, utils::string::va("%i", cg_drawFPS));

			cv = reinterpret_cast<stock::cvar_t*>(reinterpret_cast<uintptr_t>(*stock::cvar_indexes) + (0x2c * cvars::vm::cg_lagometer->handle));
			stock::Cvar_Set(cv->name, utils::string::va("%i", cg_lagometer));

			cv = reinterpret_cast<stock::cvar_t*>(reinterpret_cast<uintptr_t>(*stock::cvar_indexes) + (0x2c * cvars::vm::cg_chatHeight->handle));
			stock::Cvar_Set(cv->name, utils::string::va("%i", cg_chatHeight));
		}
	}

	void end_frame()
	{
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	}
	
	static void shutdown()
	{
		if (initialized)
		{
			ImGui_ImplOpenGL2_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			initialized = false;
		}
	}
	
	static BOOL WINAPI stub_SwapBuffers(HDC hdc)
	{
		if (!initialized)
		{
			init(hdc);
		}
		else
		{
			if (WindowFromDC(hdc) != hWnd_during_init) // Caused by Alt+Enter / vid_restart
			{
				ImGui_ImplOpenGL2_Shutdown();
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();
				init(hdc);
			}
		}

		if (displayed)
		{
			auto originalWglContext = wglGetCurrentContext();

			wglMakeCurrent(hdc, imguiWglContext);
			gui_on_frame();
			wglMakeCurrent(hdc, originalWglContext);
		}
		
		return SwapBuffers(hdc);
	}
	
	class component final : public component_interface
	{
	public:
		void* load_import(const std::string&, const std::string& function) override
		{
			if (function == "SwapBuffers")
			{
				return stub_SwapBuffers;
			}
			return nullptr;
		}

		void post_unpack() override
		{
			stock::Cmd_AddCommand("imgui", _toggle_menu);
		}
		
		void pre_destroy() override
		{
			shutdown();
		}
	};
}

REGISTER_COMPONENT(imgui::component)
#endif