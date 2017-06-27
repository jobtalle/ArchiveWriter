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
		struct Frame
		{
			uint32_t duration;
			uint32_t x, y;
			uint32_t width, height;
		};

		picojson::value config;
		inFile >> config;

		std::vector<Frame> frameDatas;

		picojson::value::object &object = config.get<picojson::object>()["frames"].get<picojson::object>();

		for(picojson::value::object::const_iterator i = object.begin(); i != object.end(); ++i)
		{
			auto frame = i->second.get<picojson::value::object>();
			auto frameRect = frame["frame"].get<picojson::value::object>();
			Frame frameData;

			frameData.duration = uint32_t(frame["duration"].get<double>());
			frameData.x = uint32_t(frameRect["x"].get<double>());
			frameData.y = uint32_t(frameRect["y"].get<double>());
			frameData.width = uint32_t(frameRect["w"].get<double>());
			frameData.height = uint32_t(frameRect["h"].get<double>());

			frameDatas.push_back(frameData);

			std::cout << frameData.duration << std::endl;
		}

		std::cout << frameDatas.size() << " frames read" << std::endl;
	}

	pixels.insert(pixels.begin(), header.begin(), header.end());

	return pixels;
}