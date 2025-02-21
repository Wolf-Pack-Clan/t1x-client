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