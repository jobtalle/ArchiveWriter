#pragma once

#include <vector>
#include <string>

namespace formatter {
	static std::vector<std::string> whitelist = {
		"png"
	};

	std::vector<char> format(std::vector<char> file, const std::string &name);

	std::vector<char> formatPng(std::vector<char> file, const std::string &name);
}