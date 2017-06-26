#include <archive/formatter.h>
#include <lodepng/lodepng.h>
#include <iostream>
#include <stdint.h>

std::vector<char> formatter::formatPng(std::vector<char> file, const std::string &name)
{
	unsigned char *result;
	uint32_t width, height;

	lodepng_decode_memory(&result, &width, &height, (unsigned char*)file.data(), file.size(), LCT_RGBA, 8);

	std::vector<char> pixels = std::vector<char>(result, result + width * height * 4);
	std::vector<char> header(sizeof(uint32_t)* 2);

	memcpy(header.data(), &width, sizeof(uint32_t));
	memcpy(header.data() + sizeof(uint32_t), &height, sizeof(uint32_t));

	pixels.insert(pixels.begin(), header.begin(), header.end());

	return pixels;
}