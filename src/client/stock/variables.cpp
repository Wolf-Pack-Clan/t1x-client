#include "pch.h"
#include "variables.h"

namespace stock
{
    const int stock::MAX_STRING_CHARS = 1024;
    const int stock::CS_SERVERINFO   = 0;
    const int stock::CS_SYSTEMINFO   = 1;

    namespace cgame_mp
    {
        WEAK adjuster<cvarTable_t> cvarTable{ 0x300851f8, BASE_CGAME_MP };
        WEAK adjuster<int (*)(int arg, ...)> syscall{ 0x30085e9c, BASE_CGAME_MP };
    }
    WEAK adjuster<int> cmd_argc{ 0x008930f0 };
    WEAK adjuster<char*> cmd_argv{ 0x00890bf0 };
    WEAK adjuster<pmove_t*> pm{ 0x30539850, BASE_CGAME_MP };
    WEAK adjuster<connstate_t> cls_state{ 0x004056be };
    WEAK adjuster<qboolean> refreshActive{ 0x40223d74, BASE_UI_MP };
    WEAK adjuster<int> bg_iNumWeapons{ 0x0010ed3c, BASE_CGAME_MP };
}