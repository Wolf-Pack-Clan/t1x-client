#include "pch.h"
#if 1
#include "movement.h"

namespace movement
{
	stock::cvar_t* sensitivity_adsScaleEnable;
	stock::cvar_t* sensitivity_adsScale;
	stock::cvar_t* sensitivity_adsScaleSniperEnable;
	stock::cvar_t* sensitivity_adsScaleSniper;
	stock::cvar_t* m_rawinput;
	
	static void Cmd_LookBack()
	{
		stock::viewangles[stock::YAW] += 180;
	}
	
	static float originalCgZoomSensitivity() // See 30032fe8
	{
		return *stock::fov_visible / cvars::vm::cg_fov->value; // See 30032fe8
	}
	
	static float scaledCgZoomSensitivity()
	{
		bool weaponIsSniper = false;

		if (!*stock::pm)
			return originalCgZoomSensitivity();
		
		int weapon = (*stock::pm)->ps->weapon;
		stock::weaponInfo_t* weaponInfo = stock::BG_GetInfoForWeapon(weapon);

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
		if (*stock::ads_progress == 1)
		{
			*stock::cg_zoomSensitivity = scaledCgZoomSensitivity();
		}
		else if (*stock::ads_progress != 0)
		{
			auto unknown = (bool*)ABSOLUTE_CGAME_MP(0x30209458); // True when zoomed out before max in, = "ads aborted"?

			if (*unknown)
				*stock::cg_zoomSensitivity = scaledCgZoomSensitivity();
			else
				*stock::cg_zoomSensitivity = originalCgZoomSensitivity();
		}
		else
		{
			*stock::cg_zoomSensitivity = originalCgZoomSensitivity();
		}
	}
	
	static __declspec(naked) void stub_cg_zoomSensitivity_calculation()
	{
		__asm
		{
			fstp st(0)
			call cg_zoomSensitivity_scale
			ret
		}
	}
	
	static void projectVelocity(const float *in, const float *normal, float *out)
	{
		float lengthSq2D;
		float adjusted;
		float newZ;
		float lengthScale;
    
		lengthSq2D = (float)(in[0] * in[0]) + (float)(in[1] * in[1]);

		if (fabs(normal[2]) < 0.001 || lengthSq2D == 0.0)
		{
			out[0] = in[0];
			out[1] = in[1];
			out[2] = in[2];
		}
		else
		{
			newZ = (float)-(float)((float)(in[0] * normal[0]) + (float)(in[1] * normal[1])) / normal[2];
			adjusted = in[1];
			lengthScale = sqrt((float)((float)(in[2] * in[2]) + lengthSq2D) / (float)((float)(newZ * newZ) + lengthSq2D));

			if (lengthScale < 1.0 || newZ < 0.0 || in[2] > 0.0)
			{
				out[0] = lengthScale * in[0];
				out[1] = lengthScale * adjusted;
				out[2] = lengthScale * newZ;
			}
		}
	}
	
	static uint32_t projectOrClip(stock::vec3_t in, stock::vec3_t normal, stock::vec3_t out, float overbounce)
	{
		char* cl_gameState_stringData = (char*)0x01436a7c;
		int* cl_gameState_stringOffsets = (int*)0x1434A7C;
		char* systemInfo = cl_gameState_stringData + cl_gameState_stringOffsets[stock::CS_SYSTEMINFO];

		/*std::stringstream ss;
		ss << "####### in[0]: " << in[0] << std::endl;
		ss << "####### normal[0]: " << normal[0] << std::endl;
		ss << "####### out[0]: " << out[0] << std::endl;
		ss << "####### overbounce: " << overbounce << std::endl;
		OutputDebugString(ss.str().c_str());*/
		
		if (atoi(stock::Info_ValueForKey(systemInfo, "jump_bounceEnable")))
			projectVelocity(in, normal, out);
		else
			stock::PM_ClipVelocity(in, normal, out, overbounce);
		
		return ABSOLUTE_CGAME_MP(0x3000D830);
	}
	
	static __declspec(naked) void stub_PM_StepSlideMove_PM_ClipVelocity()
	{
		__asm
		{
			push esi; // out
			push ecx; // normal
			push edx; // in
			call projectOrClip;
			add esp, 0xc;
			
			push eax;
			retn;
		}
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			sensitivity_adsScaleEnable = stock::Cvar_Get("sensitivity_adsScaleEnable", "0", stock::CVAR_ARCHIVE);
			sensitivity_adsScale = stock::Cvar_Get("sensitivity_adsScale", "1", stock::CVAR_ARCHIVE);
			sensitivity_adsScaleSniperEnable = stock::Cvar_Get("sensitivity_adsScaleSniperEnable", "0", stock::CVAR_ARCHIVE);
			sensitivity_adsScaleSniper = stock::Cvar_Get("sensitivity_adsScaleSniper", "1", stock::CVAR_ARCHIVE);
			m_rawinput = stock::Cvar_Get("m_rawinput", "0", stock::CVAR_ARCHIVE);
			
			stock::Cmd_AddCommand("lookback", Cmd_LookBack);
		}

		void post_cgame() override
		{
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032fe8), stub_cg_zoomSensitivity_calculation);
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x3000d82b), stub_PM_StepSlideMove_PM_ClipVelocity);
		}
	};
}

REGISTER_COMPONENT(movement::component)
#endif