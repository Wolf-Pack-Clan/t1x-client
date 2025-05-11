#include "pch.h"
#if 1
#include "view.h"

namespace view
{
    bool g_legacyStyle_client;
    std::map<std::string, std::map<std::string, stock::WeaponProperties>> weapons_properties;
	
    utils::hook::detour hook_CG_Respawn;
	utils::hook::detour hook_CL_PlayDemo_f;
    utils::hook::detour hook_CL_SystemInfoChanged;
	
	static float scaledFOV(float fov)
	{
		/*int* flag = (int*)ABSOLUTE_CGAME_MP(0x302071dc); // Might be cg.snap->ps.eFlags
		if (*flag & stock::EF_MG42_ACTIVE)
			return 55;

		if (cg_fovScaleEnable->integer)
			return fov * cg_fovScale->value;

		return fov;//*/
	}
	
	static __declspec(naked) void stub_CG_CalcFov_return()
	{
		/*__asm
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
		}//*/
	}
    
    void toggleLegacyStyle()
    {
        printf("#### toggleLegacyStyle\n");
        const char* g_legacyStyle = stock::Info_ValueForKey(cs1, "g_legacyStyle");
        if (!*g_legacyStyle) {
            printf("#### toggleLegacyStyle: g_legacyStyle undefined\n");
            return;
        }
    
        bool g_legacyStyle_server = (atoi(g_legacyStyle) == 1) ? true : false;
        if (g_legacyStyle_client == g_legacyStyle_server) {
            printf("#### toggleLegacyStyle: g_legacyStyle unchanged\n");
            return;
        }
        
        printf("#### toggleLegacyStyle: setting weapon properties\n");
        int32_t id_kar98k_sniper;
        id_kar98k_sniper = stock::BG_GetWeaponIndexForName("kar98k_sniper_mp");
        printf("#### toggleLegacyStyle: id_kar98k_sniper = %d\n", id_kar98k_sniper);
        stock::weaponinfo_t* weapon_kar98k_sniper = stock::BG_GetWeaponDef(id_kar98k_sniper);
        int32_t id_springfield = stock::BG_GetWeaponIndexForName("springfield_mp");
        stock::weaponinfo_t* weapon_springfield = stock::BG_GetWeaponDef(id_springfield);
        int32_t id_mosin_nagant_sniper = stock::BG_GetWeaponIndexForName("mosin_nagant_sniper_mp");
        stock::weaponinfo_t* weapon_mosin_nagant_sniper = stock::BG_GetWeaponDef(id_mosin_nagant_sniper);

        printf("#### toggleLegacyStyle: created weapon definitions\n");
        if (weapon_kar98k_sniper)
        {
            printf("#### toggleLegacyStyle: weapon_kar98k_sniper\n");
            const stock::WeaponProperties* properties_kar98k_sniper = nullptr;
            if (g_legacyStyle_server) {
                printf("#### toggleLegacyStyle: setting legacy properties for weapon_kar98k_sniper\n");
                properties_kar98k_sniper = &weapons_properties[weapon_kar98k_sniper->name]["legacy"];
            }
            else {
                printf("#### toggleLegacyStyle: setting default properties for weapon_kar98k_sniper\n");
                properties_kar98k_sniper = &weapons_properties[weapon_kar98k_sniper->name]["default"];
            }

            
            weapon_kar98k_sniper->adsTransInTime = properties_kar98k_sniper->adsTransInTime;
            printf("#### toggleLegacyStyle: adsTransInTime weapon_kar98k_sniper\n");
            weapon_kar98k_sniper->OOPosAnimLength[0] = 1.0 / (float)weapon_kar98k_sniper->adsTransInTime;
            printf("#### toggleLegacyStyle: OOPosAnimLength[0] weapon_kar98k_sniper\n");
            weapon_kar98k_sniper->adsZoomInFrac = properties_kar98k_sniper->adsZoomInFrac;
            printf("#### toggleLegacyStyle: adsZoomInFrac weapon_kar98k_sniper\n");
            weapon_kar98k_sniper->idleCrouchFactor = properties_kar98k_sniper->idleCrouchFactor;
            printf("#### toggleLegacyStyle: idleCrouchFactor weapon_kar98k_sniper\n");
            weapon_kar98k_sniper->idleProneFactor = properties_kar98k_sniper->idleProneFactor;
            printf("#### toggleLegacyStyle: idleProneFactor weapon_kar98k_sniper\n");
            weapon_kar98k_sniper->rechamberWhileAds = properties_kar98k_sniper->rechamberWhileAds;
            printf("#### toggleLegacyStyle: rechamberWhileAds weapon_kar98k_sniper\n");
            weapon_kar98k_sniper->adsViewErrorMin = properties_kar98k_sniper->adsViewErrorMin;
            printf("#### toggleLegacyStyle: adsViewErrorMin weapon_kar98k_sniper\n");
            weapon_kar98k_sniper->adsViewErrorMax = properties_kar98k_sniper->adsViewErrorMax;
            printf("#### toggleLegacyStyle: adsViewErrorMax weapon_kar98k_sniper\n");
            printf("#### toggleLegacyStyle: done setting properties for weapon_kar98k_sniper\n");
        }
    
        if (weapon_mosin_nagant_sniper)
        {
            const stock::WeaponProperties* properties_mosin_nagant_sniper = nullptr;
            if (g_legacyStyle_server)
                properties_mosin_nagant_sniper = &weapons_properties[weapon_mosin_nagant_sniper->name]["legacy"];
            else
                properties_mosin_nagant_sniper = &weapons_properties[weapon_mosin_nagant_sniper->name]["default"];
            weapon_mosin_nagant_sniper->reloadAddTime = properties_mosin_nagant_sniper->reloadAddTime;
            weapon_mosin_nagant_sniper->adsTransInTime = properties_mosin_nagant_sniper->adsTransInTime;
            weapon_mosin_nagant_sniper->OOPosAnimLength[0] = 1.0 / (float)weapon_mosin_nagant_sniper->adsTransInTime;
            weapon_mosin_nagant_sniper->adsZoomInFrac = properties_mosin_nagant_sniper->adsZoomInFrac;
            weapon_mosin_nagant_sniper->idleCrouchFactor = properties_mosin_nagant_sniper->idleCrouchFactor;
            weapon_mosin_nagant_sniper->idleProneFactor = properties_mosin_nagant_sniper->idleProneFactor;
            weapon_mosin_nagant_sniper->rechamberWhileAds = properties_mosin_nagant_sniper->rechamberWhileAds;
            weapon_mosin_nagant_sniper->adsViewErrorMin = properties_mosin_nagant_sniper->adsViewErrorMin;
            weapon_mosin_nagant_sniper->adsViewErrorMax = properties_mosin_nagant_sniper->adsViewErrorMax;
        }
    
        if (weapon_springfield)
        {
            const stock::WeaponProperties* properties_springfield = nullptr;
            if (g_legacyStyle_server)
                properties_springfield = &weapons_properties[weapon_springfield->name]["legacy"];
            else
                properties_springfield = &weapons_properties[weapon_springfield->name]["default"];
            weapon_springfield->adsTransInTime = properties_springfield->adsTransInTime;
            weapon_springfield->OOPosAnimLength[0] = 1.0 / (float)weapon_springfield->adsTransInTime;
            weapon_springfield->adsZoomInFrac = properties_springfield->adsZoomInFrac;
            weapon_springfield->idleCrouchFactor = properties_springfield->idleCrouchFactor;
            weapon_springfield->idleProneFactor = properties_springfield->idleProneFactor;
            weapon_springfield->rechamberWhileAds = properties_springfield->rechamberWhileAds;
            weapon_springfield->adsViewErrorMin = properties_springfield->adsViewErrorMin;
            weapon_springfield->adsViewErrorMax = properties_springfield->adsViewErrorMax;
        }
    
        g_legacyStyle_client = g_legacyStyle_server;
    }

    static void stub_CL_SystemInfoChanged()
    {
        printf("#### stub_CL_SystemInfoChanged\n");
        hook_CL_SystemInfoChanged.invoke();
        toggleLegacyStyle();
    }
	
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			//record_respawn = stock::Cvar_Get("record_respawn", "0", stock::CVAR_ARCHIVE);

			//hook_CL_PlayDemo_f.create(0x0040eb40, stub_CL_PlayDemo_f);

            if (weapons_properties.empty())
            {
                weapons_properties["kar98k_sniper_mp"]["default"] = { 199, 449, 0.1f, 0.6f, 0.2f, 0, 1.2f, 1.4f };
                weapons_properties["kar98k_sniper_mp"]["legacy"] = { 199, 299, 0.42f, 0.2f, 0.085f, 1, 0.0f, 0.0f };

                weapons_properties["mosin_nagant_sniper_mp"]["default"] = { 1339, 449, 0.1f, 0.6f, 0.2f, 0, 1.2f, 1.4f };
                weapons_properties["mosin_nagant_sniper_mp"]["legacy"] = { 339, 299, 0.42f, 0.2f, 0.085f, 1, 0.0f, 0.0f };

                weapons_properties["springfield_mp"]["default"] = { 199, 449, 0.1f, 0.6f, 0.2f, 0, 1.2f, 1.4f };
                weapons_properties["springfield_mp"]["legacy"] = { 199, 299, 0.5f, 0.2f, 0.085f, 1, 0.0f, 0.0f };
                /*
                springfield_mp adsZoomInFrac in 1.1 patch weapon file = 0.05.
                There must be an error somewhere. Now replacing by 0.5 to fix slowness.
                */
            }

            g_legacyStyle_client = true;
            toggleLegacyStyle();
		}

		void post_cgame() override
		{
			//utils::hook::jump(ABSOLUTE_CGAME_MP(0x30032f2a), stub_CG_CalcFov_return);
			
			//hook_CG_Respawn.create(ABSOLUTE_CGAME_MP(0x30028a70), stub_CG_Respawn);
            hook_CL_SystemInfoChanged.create(0x00418b80, stub_CL_SystemInfoChanged);
		}
	};
}

//REGISTER_COMPONENT(view::component)
extern "C" __declspec(dllexport) component_interface* create_component()
{
    OutputDebugStringA("Creating view component\n");
    return new view::component();
}

#endif