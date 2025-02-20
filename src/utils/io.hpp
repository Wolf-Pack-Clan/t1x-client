#pragma once

#include <string>

namespace utils::io
{
	bool file_exists(const std::string& file);
	bool read_file(const std::string& file, std::string* data);
	std::string read_file(const std::string& file);
}