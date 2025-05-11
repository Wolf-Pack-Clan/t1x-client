#include "pch.h"
#include "t1x_core.h"

extern "C" T1X_API DWORD address_cgame_mp = 0;
extern "C" T1X_API DWORD address_ui_mp = 0;

/*static HMODULE WINAPI stub_LoadLibraryA(LPCSTR lpLibFileName)
{
    auto ret = LoadLibraryA(lpLibFileName);
    auto hModule_address = (DWORD)GetModuleHandleA(lpLibFileName);

    if (lpLibFileName != NULL)
    {
        auto fileName = PathFindFileNameA(lpLibFileName);
        if (!strcmp(fileName, "uo_cgame_mp_x86.dll"))
        {
            address_cgame_mp = hModule_address;
            component_loader::post_cgame();
        }
        else if (!strcmp(fileName, "uo_ui_mp_x86.dll"))
        {
            address_ui_mp = hModule_address;
            component_loader::post_ui_mp();
        }
    }
    return ret;
}//*/