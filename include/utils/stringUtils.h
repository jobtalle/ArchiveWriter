#pragma once

#include <string>

namespace utils
{
	inline std::string getExtension(const std::string &str)
	{
		return str.substr(str.find_last_of('.') + 1);
	}

	inline bool existsIn(const std::string &str, const std::vector<std::string> &vector)
	{
		for(const std::string current : vector)
			if(current.length() >= str.length() && current.compare(current.length() - str.length(), str.length(), str) == 0)
				return true;

		return false;
	}
}