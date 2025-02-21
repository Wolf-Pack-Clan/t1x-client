#include "pch.h"
#if 1
#include "shared.h"
#include "imgui.h"

namespace imgui
{
	bool initialized = false;
	bool displayed = false;
	bool waitForMenuKeyRelease = false;

	HGLRC imguiWglContext;
	HWND hWnd_during_init;

	bool sensitivity_adsScaleEnable = false;
	float sensitivity_adsScale = 0.f;
	bool sensitivity_adsScaleSniperEnable = false;
	float sensitivity_adsScaleSniper = 0.f;
	bool cg_drawDisconnect = true;
	bool cg_drawWeaponSelect = true;
	bool cg_drawFPS = false;
	int cg_chatHeight = 0;
	int con_boldgamemessagetime = 0;
	bool cg_lagometer = false;
	bool cl_allowDownload = false;
	bool m_rawinput = false;
	float cg_fov = 0.f;
	bool cg_fovScaleEnable = false;
	float cg_fovScale = 0.f;
	
	void toggle_menu_flag()
	{
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
		ImGuiStyle& style = ImGui::GetStyle();

		io.IniFilename = "iw1x_imgui.ini";
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 16.0f);
		style.WindowPadding.x += 8;
		style.WindowPadding.y += 7;

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
		cg_drawFPS = ui::cg_drawFPS->integer;
		cg_chatHeight = ui::cg_chatHeight->integer;
		con_boldgamemessagetime = ui::con_boldgamemessagetime->integer;
		cg_lagometer = ui::cg_lagometer->integer;
		cl_allowDownload = !security::cl_allowDownload->integer;
		m_rawinput = movement::m_rawinput->integer;
		cg_fov = view::cg_fov->value;
		cg_fovScaleEnable = view::cg_fovScaleEnable->integer;
		cg_fovScale = view::cg_fovScale->value;
	}

	void draw_menu()
	{
		menu_loads_settings();

		ImGui::SetNextWindowSize(ImVec2(300, 0));
		ImGui::SetNextWindowPos(ImVec2(25, 80), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowFocus();
		ImGui::Begin(MOD_NAME, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		//// Security

		ImGui::SeparatorText("Security");
		ImGui::Checkbox("Deny downloads", &cl_allowDownload);
		////

		ImGui::Dummy(ImVec2(0, 10)); // Spacing

		//// UI

		ImGui::SeparatorText("UI");
		ImGui::Checkbox("FPS", &cg_drawFPS);
		ImGui::Checkbox("Lagometer", &cg_lagometer);
		ImGui::Checkbox("\"Connection Interrupted\"", &cg_drawDisconnect);
		ImGui::Checkbox("Weapon selection", &cg_drawWeaponSelect);

		ImGui::Spacing();

		ImGui::Text("Chat lines");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::SliderInt("##slider_cg_chatHeight", &cg_chatHeight, 0, 8, "%i", ImGuiSliderFlags_NoInput);

		ImGui::Spacing();

		ImGui::Text("Middle messages seconds");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::SliderInt("##slider_con_boldgamemessagetime", &con_boldgamemessagetime, 0, 8, "%i", ImGuiSliderFlags_NoInput);
		////

		//// View

		ImGui::SeparatorText("View");
		ImGui::Text("FOV");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::SliderFloat("##slider_cg_fov", &cg_fov, 80.f, 95.f, "%.2f", ImGuiSliderFlags_NoInput);
		
		ImGui::Spacing();
		
		ImGui::Checkbox("FOV scale", &cg_fovScaleEnable);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (!cg_fovScaleEnable)
			ImGui::BeginDisabled();
		ImGui::SliderFloat("##slider_cg_fovScale", &cg_fovScale, 1.f, 1.4f, "%.2f", ImGuiSliderFlags_NoInput);
		if (!cg_fovScaleEnable)
			ImGui::EndDisabled();
		////
		
		ImGui::Dummy(ImVec2(0, 10)); // Spacing

		//// Movement

		ImGui::SeparatorText("Movement");
		ImGui::Checkbox("Raw mouse input", &m_rawinput);

		ImGui::Spacing();

		// Sensitivity multiplier
		ImGui::Checkbox("Sensitivity multiplier", &sensitivity_adsScaleEnable);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (!sensitivity_adsScaleEnable)
			ImGui::BeginDisabled();
		ImGui::SliderFloat("##slider_sensitivity_adsScale", &sensitivity_adsScale, 0.15f, 1.f, "%.2f", ImGuiSliderFlags_NoInput);
		if (!sensitivity_adsScaleEnable)
			ImGui::EndDisabled();

		ImGui::Spacing();

		// Sensitivity sniper multiplier
		ImGui::Checkbox("Sensitivity sniper multiplier", &sensitivity_adsScaleSniperEnable);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (!sensitivity_adsScaleSniperEnable)
			ImGui::BeginDisabled();
		ImGui::SliderFloat("##slider_sensitivity_adsScaleSniper", &sensitivity_adsScaleSniper, 0.15f, 1.f, "%.2f", ImGuiSliderFlags_NoInput);
		if (!sensitivity_adsScaleSniperEnable)
			ImGui::EndDisabled();
		////

		ImGui::End();
		menu_updates_settings();
	}

	void menu_updates_settings()
	{
		stock::Cvar_Set(movement::sensitivity_adsScaleEnable->name, sensitivity_adsScaleEnable ? "1" : "0");
		stock::Cvar_Set(movement::sensitivity_adsScale->name, utils::string::va("%.2f", sensitivity_adsScale));
		stock::Cvar_Set(movement::sensitivity_adsScaleSniperEnable->name, sensitivity_adsScaleSniperEnable ? "1" : "0");
		stock::Cvar_Set(movement::sensitivity_adsScaleSniper->name, utils::string::va("%.2f", sensitivity_adsScaleSniper));
		stock::Cvar_Set(ui::cg_drawDisconnect->name, cg_drawDisconnect ? "1" : "0");
		stock::Cvar_Set(ui::cg_drawWeaponSelect->name, cg_drawWeaponSelect ? "1" : "0");
		stock::Cvar_Set(ui::cg_drawFPS->name, cg_drawFPS ? "1" : "0");
		stock::Cvar_Set(ui::cg_chatHeight->name, utils::string::va("%i", cg_chatHeight));
		stock::Cvar_Set(ui::con_boldgamemessagetime->name, utils::string::va("%i", con_boldgamemessagetime));
		stock::Cvar_Set(ui::cg_lagometer->name, cg_lagometer ? "1" : "0");
		stock::Cvar_Set(security::cl_allowDownload->name, cl_allowDownload ? "0" : "1");
		stock::Cvar_Set(movement::m_rawinput->name, m_rawinput ? "1" : "0");
		stock::Cvar_Set(view::cg_fov->name, utils::string::va("%.2f", cg_fov));
		stock::Cvar_Set(view::cg_fovScaleEnable->name, cg_fovScaleEnable ? "1" : "0");
		stock::Cvar_Set(view::cg_fovScale->name, utils::string::va("%.2f", cg_fovScale));
	}

	void end_frame()
	{
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	}
	
	void shutdown()
	{
		if (initialized)
		{
			ImGui_ImplOpenGL2_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			initialized = false;
		}
	}
	
	BOOL WINAPI SwapBuffers_stub(HDC hdc)
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
				return SwapBuffers_stub;
			}
			return nullptr;
		}
		
		void pre_destroy() override
		{
			shutdown();
		}
	};
}

REGISTER_COMPONENT(imgui::component)
#endif