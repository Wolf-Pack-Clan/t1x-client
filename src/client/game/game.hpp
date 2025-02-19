#pragma once

#include "types.hpp"

constexpr auto BASE_CGAME_MP = 0x30000000;
constexpr auto BASE_UI_MP = 0x40000000;

extern DWORD address_cgame_mp;
extern DWORD address_ui_mp;

#define ABSOLUTE_CGAME_MP(relative) (address_cgame_mp + (relative - BASE_CGAME_MP))
#define ABSOLUTE_UI_MP(relative)	(address_ui_mp + (relative - BASE_UI_MP))

namespace game
{
	namespace environment
	{
		std::string get_client_filename();
		extern bool mohaa;
	}

	template <typename T>
	class symbol
	{
	public:
		symbol(const size_t mp_address, const ptrdiff_t offset = 0) :
			mp_object(reinterpret_cast<T*>(mp_address)),
			offset(offset)
		{
		}

		T* get() const
		{
			T* ptr = mp_object;
			uintptr_t base_address = 0;

			switch (offset)
			{
			case BASE_CGAME_MP:
				base_address = address_cgame_mp;
				break;
			case BASE_UI_MP:
				base_address = address_ui_mp;
				break;
			default:
				return ptr;
			}

			return reinterpret_cast<T*>(base_address + (reinterpret_cast<uintptr_t>(ptr) - offset));
		}

		operator T*() const
		{
			return this->get();
		}

		T* operator->() const
		{
			return this->get();
		}
	private:
		T* mp_object;
		ptrdiff_t offset;
	};
	
	int Cmd_Argc();
	char* Cmd_Argv(int arg);
	game::weaponInfo_t* BG_GetInfoForWeapon(int weaponNum);
	const char* Info_ValueForKey(const char* buffer, const char* key);
}

#include "variables.hpp"
#include "functions.hpp"