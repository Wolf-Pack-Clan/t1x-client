#include <pch.hpp>
#if 1
#include <shared.hpp>
#include <hook.hpp>
#include "loader/component_loader.hpp"

#include "view.hpp"

namespace view
{
	stock::cvar_t* cg_fov;
	stock::cvar_t* cg_fovScaleEnable;
	stock::cvar_t* cg_fovScale;
	
	static float scaledFOV(float fov)
	{
		int* flag = (int*)ABSOLUTE_CGAME_MP(0x302071dc); // Might be cg.snap->ps.eFlags
		if (*flag & stock::EF_MG42_ACTIVE)
			return 55;

		if (cg_fovScaleEnable->integer)
			return fov * cg_fovScale->value;

		return fov;
	}
	
	static __declspec(naked) void CG_CalcFov_return_stub()
	{
		__asm
		{
			sub esp, 4;
			fstp dword ptr[esp];			
			push dword ptr[esp];
			call scaledFOV;
			add esp, 4;

			fstp dword ptr[esp];
			fld dword ptr[esp];
			add esp, 4;

			pop ecx;
			ret;
		}
	}
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			cg_fov = stock::Cvar_Get("cg_fov", "80", stock::CVAR_ARCHIVE);
			cg_fovScaleEnable = stock::Cvar_Get("cg_fovScaleEnable", "0", stock::CVAR_ARCHIVE);
			cg_fovScale = stock::Cvar_Get("cg_fovScale", "1", stock::CVAR_ARCHIVE);
		}

		void post_cgame() override
		{
			utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032f2a), CG_CalcFov_return_stub);
		}
	};
}

REGISTER_COMPONENT(view::component)
#endif