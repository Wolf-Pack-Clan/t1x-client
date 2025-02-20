#include "io.hpp"
#include "nt.hpp"
#include <fstream>

namespace utils::io
{
	bool file_exists(const std::string& file)
	{
		return std::ifstream(file).good();
	}

	std::string read_file(const std::string& file)
	{
		std::string data;
		read_file(file, &data);
		return data;
	}

	bool read_file(const std::string& file, std::string* data)
	{
		if (!data) return false;
		data->clear();

		if (file_exists(file))
		{
			std::ifstream stream(file, std::ios::binary);
			if (!stream.is_open()) return false;

			stream.seekg(0, std::ios::end);
			const std::streamsize size = stream.tellg();
			stream.seekg(0, std::ios::beg);

			if (size > -1)
			{
				data->resize(static_cast<uint32_t>(size));
				stream.read(data->data(), size);
				stream.close();
				return true;
			}
		}

		return false;
	}
}