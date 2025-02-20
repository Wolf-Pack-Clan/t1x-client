#include <std_include.hpp>
#include "game.hpp"

namespace game
{
	namespace environment
	{
		bool mohaa = false;
		std::string get_client_filename()
		{
			if (mohaa)
				return "mohaa.exe";
			else
				return "CoDMP.exe";
		}
	}
}