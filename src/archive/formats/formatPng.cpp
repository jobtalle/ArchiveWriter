#include <archive/formatter.h>
#include <lodepng/lodepng.h>
#include <iostream>
#include <stdint.h>
#include <utils/stringUtils.h>
#include <picojson/picojson.h>
#include <fstream>

std::vector<char> formatter::formatPng(std::vector<char> file, const std::string &name)
{
	struct HeaderFrame
	{
		uint32_t duration;
		uint32_t x, y;
	};

	struct Header
	{
		uint32_t width, height;
		uint32_t frameWidth, frameHeight;
		uint32_t frameCount;
	};

	unsigned char *result;
	Header header;
	HeaderFrame *frames = nullptr;
	const std::string json = name.substr(0, name.length() - utils::getExtension(name).length()) + "json";

	// Load pixels from PNG
	lodepng_decode_memory(&result, &header.width, &header.height, (unsigned char*)file.data(), file.size(), LCT_RGBA, 8);

	// Read JSON configs
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

		auto &object = config.get<picojson::value::object>()["frames"].get<picojson::object>();

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
		}

		header.frameCount = uint32_t(frameDatas.size());
		header.frameWidth = frameDatas.at(0).width;
		header.frameHeight = frameDatas.at(0).height;

		frames = new HeaderFrame[header.frameCount];

		for(uint32_t i = 0; i < header.frameCount; ++i)
		{
			frames[i].duration = frameDatas.at(i).duration;
			frames[i].x = frameDatas.at(i).x;
			frames[i].y = frameDatas.at(i).y;
		}
	}
	else
	{
		header.frameCount = 1;
		header.frameWidth = header.width;
		header.frameHeight = header.height;
	}

	// Copy data to result
	std::vector<char> pixels = std::vector<char>(result, result + header.width * header.height * 4);
	std::vector<char> headerBytes = std::vector<char>(
		sizeof(Header) +
		(header.frameCount > 1?header.frameCount:0) * sizeof(HeaderFrame));

	memcpy(headerBytes.data(), &header, sizeof(Header));

	if(header.frameCount > 1)
		for(uint32_t i = 0; i < header.frameCount; ++i)
			memcpy(headerBytes.data() + sizeof(Header) + sizeof(HeaderFrame) * i, frames + i, sizeof(HeaderFrame));

	pixels.insert(pixels.begin(), headerBytes.begin(), headerBytes.end());

	delete frames;

	return pixels;
}