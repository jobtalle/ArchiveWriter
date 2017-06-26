#include <archive/formatter.h>
#include <utils/stringUtils.h>
#include <iostream>

std::vector<char> formatter::format(std::vector<char> file, const std::string &name)
{
	const std::string extension = utils::getExtension(name);

	if(extension == "png")
		return formatPng(file, name);
	else
		return file;
}