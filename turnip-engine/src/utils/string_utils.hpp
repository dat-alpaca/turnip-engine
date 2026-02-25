#pragma once
#include <algorithm>
#include <string>

namespace tur
{
	inline void to_lower(std::string& string)
	{
		std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) { return std::tolower(c); });
	}

	inline void to_upper(std::string& string)
	{
		std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) { return std::toupper(c); });
	}
}