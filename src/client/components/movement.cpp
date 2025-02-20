#include <std_include.hpp>
#if 1
#include <hook.hpp>
#include "loader/component_loader.hpp"
#include "stock/game.hpp"

namespace movement
{
	game::cvar_t* sensitivity_adsScaleEnable;
	game::cvar_t* sensitivity_adsScale;
	game::cvar_t* sensitivity_adsScaleSniperEnable;
	game::cvar_t* sensitivity_adsScaleSniper;
	game::cvar_t* m_rawinput;
	
	static void Cmd_LookBack()
	{
		game::viewangles[YAW] += 180;
	}
	
	static float originalCgZoomSensitivity()
	{
		return *game::fov_visible / *game::cg_fov_value; // See 30032fe8
	}
	
	static float scaledCgZoomSensitivity()
	{
		if (!*game::pm)
			return originalCgZoomSensitivity();

		bool weaponIsSniper = false;
		
		int weapon = (*game::pm)->ps->weapon;
		game::weaponInfo_t* weaponInfo = game::BG_GetInfoForWeapon(weapon);

		if (*weaponInfo->adsOverlayShader)
			weaponIsSniper = true;
		
		if (weaponIsSniper)
		{
			if (sensitivity_adsScaleSniperEnable->integer)
				return originalCgZoomSensitivity() * sensitivity_adsScaleSniper->value;
			else
				return originalCgZoomSensitivity();
		}
		else if (sensitivity_adsScaleEnable->integer)
			return originalCgZoomSensitivity() * sensitivity_adsScale->value;
		else
			return originalCgZoomSensitivity();
	}

	static void cg_zoomSensitivity_scale() // See 30032e20
	{
		if (*game::ads_progress == 1)
		{
			*game::cg_zoomSensitivity = scaledCgZoomSensitivity();
		}
		else if (*game::ads_progress != 0)
		{
			auto unknown = (bool*)ABSOLUTE_CGAME_MP(0x30209458); // True when zoomed out before max in, = "ads aborted"?

			if (*unknown)
				*game::cg_zoomSensitivity = scaledCgZoomSensitivity();
			else
				*game::cg_zoomSensitivity = originalCgZoomSensitivity();
		}
		else
		{
			*game::cg_zoomSensitivity = originalCgZoomSensitivity();
		}
	}
	
	static __declspec(naked) void cg_zoomSensitivity_calculation_stub()
	{
		__asm
		{
			fstp st(0)
			call cg_zoomSensitivity_scale
			ret
		}
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			sensitivity_adsScaleEnable = game::Cvar_Get("sensitivity_adsScaleEnable", "0", CVAR_ARCHIVE);
			sensitivity_adsScale = game::Cvar_Get("sensitivity_adsScale", "1", CVAR_ARCHIVE);
			sensitivity_adsScaleSniperEnable = game::Cvar_Get("sensitivity_adsScaleSniperEnable", "0", CVAR_ARCHIVE);
			sensitivity_adsScaleSniper = game::Cvar_Get("sensitivity_adsScaleSniper", "1", CVAR_ARCHIVE);
			m_rawinput = game::Cvar_Get("m_rawinput", "0", CVAR_ARCHIVE);
			
			game::Cmd_AddCommand("lookback", Cmd_LookBack);
		}

		void post_cgame() override
		{
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032fe8), cg_zoomSensitivity_calculation_stub);
		}
	};
}

REGISTER_COMPONENT(movement::component)
#endif