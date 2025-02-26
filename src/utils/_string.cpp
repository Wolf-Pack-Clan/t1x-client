#include "_string.h"

namespace utils::string
{
	const char* va(const char* fmt, ...)
	{
		static thread_local va_provider<8, 256> provider;

		va_list ap;
		va_start(ap, fmt);

		const char* result = provider.get(fmt, ap);

		va_end(ap);
		return result;
	}

	void clean(const char* in, char* out, int max, bool removeColors)
	{
		if (!in || !out || !max) return;

		max--;
		auto current = 0;
		while (*in != 0 && current < max)
		{
			const auto color_index = (*(in + 1) - 48) >= 0xC ? 7 : (*(in + 1) - 48);

			if (removeColors && *in == '^' && (color_index != 7 || *(in + 1) == '7'))
			{
				++in;
			}
			else if (*in >= 0x20 && *in <= 0x7E)
			{
				*out = *in;
				++out;
				++current;
			}

			++in;
		}
		*out = '\0';
	}

	std::string clean(const std::string& string, bool removeColors)
	{
		std::string new_string;
		new_string.resize(string.size() + 1, 0);
		clean(string.data(), new_string.data(), static_cast<int>(new_string.size()), removeColors);
		return new_string.data();
	}

	std::string convert(const std::wstring& wstr)
	{
		std::string result;
		result.reserve(wstr.size());

		for (const auto& chr : wstr)
		{
			result.push_back(static_cast<char>(chr));
		}

		return result;
	}

	std::wstring convert(const std::string& str)
	{
		std::wstring result;
		result.reserve(str.size());

		for (const auto& chr : str)
		{
			result.push_back(static_cast<wchar_t>(chr));
		}

		return result;
	}
}