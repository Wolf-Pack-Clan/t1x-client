#include "pch.h"
#if 1
#include "shared.h"

#include "hook.h"

#include "loader/component_loader.h"

namespace fixes
{
    uintptr_t pfield_charevent_return = 0x40dd47; // 0040dd47 for uo
    uintptr_t pfield_charevent_continue = 0x40dcf3; // 0040dcf3 for uo
    static __declspec(naked) void stub_Field_CharEvent_ignore_console_char()
    {
        __asm
        {
            cmp ebx, 20h;
            jge check;
            jmp pfield_charevent_return;

        check:
            cmp ebx, 126;
            jl checked;
            jmp pfield_charevent_return;

        checked:
            jmp pfield_charevent_continue;
        }
    }
    
    static char* stub_CL_SetServerInfo_hostname_strncpy(char* dest, const char* src, int destsize)
	{
#pragma warning(push)
#pragma warning(disable: 4996)
		strncpy(dest, utils::string::clean(src, false).c_str(), destsize); // destsize is already max-1 (=31), so not using _TRUNCATE, not to lose a char
#pragma warning(pop)
		return dest;
	}

    class component final : public component_interface
    {
    public:
        void post_unpack() override
        {
            utils::hook::jump(0x40dcee, stub_Field_CharEvent_ignore_console_char);
            utils::hook::call(0x4151b9, stub_CL_SetServerInfo_hostname_strncpy);
        }
    };
}

REGISTER_COMPONENT(fixes::component)
#endif
