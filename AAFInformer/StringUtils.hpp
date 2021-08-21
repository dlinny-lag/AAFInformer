#pragma once
#include <cctype>
#include <algorithm>

struct SU
{
	static std::string ToUpper(const std::string& s)
	{
		// All tags are in ASCII, so we don't care about locale during case conversion
		std::string copy = s;
		std::transform(copy.begin(), copy.end(), copy.begin(), [](unsigned char c) { return std::toupper(c); });
		return copy;
	}
	static std::string ToUpper(const char* s)
	{
		// All tags are in ASCII, so we don't care about locale during case conversion
		std::string copy = s;
		std::transform(copy.begin(), copy.end(), copy.begin(), [](unsigned char c) { return std::toupper(c); });
		return copy;
	}
};