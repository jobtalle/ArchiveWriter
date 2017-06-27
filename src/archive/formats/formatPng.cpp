#include <archive/formatter.h>
#include <lodepng/lodepng.h>
#include <iostream>
#include <stdint.h>
#include <utils/stringUtils.h>
#include <picojson/picojson.h>
#include <fstream>

std::vector<char> formatter::formatPng(std::vector<char> file, const std::string &name)
{
	unsigned char *result;
	uint32_t width, height;
	const std::string json = name.substr(0, name.length() - utils::getExtension(name).length()) + "json";

	lodepng_decode_memory(&result, &width, &height, (unsigned char*)file.data(), file.size(), LCT_RGBA, 8);

	std::vector<char> pixels = std::vector<char>(result, result + width * height * 4);
	std::vector<char> header(sizeof(uint32_t)* 2);

	memcpy(header.data(), &width, sizeof(uint32_t));
	memcpy(header.data() + sizeof(uint32_t), &height, sizeof(uint32_t));

	std::ifstream inFile(json);

	if(inFile.is_open())
	{
		picojson::value config;
		inFile >> config;

		std::vector<float> frameLengths;

		picojson::value::object &object = config.get<picojson::object>()["frames"].get<picojson::object>();

		for(picojson::value::object::const_iterator i = object.begin(); i != object.end(); ++i)
		{
			picojson::value::object frame = i->second.get<picojson::value::object>();

			frameLengths.push_back(1000.0f / float(frame["duration"].get<double>()));

			std::cout << frame["duration"] << std::endl;
		}

		std::cout << frameLengths.size() << " frames read" << std::endl;
	}

	pixels.insert(pixels.begin(), header.begin(), header.end());

	return pixels;
}