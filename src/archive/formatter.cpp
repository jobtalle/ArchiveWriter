#include <archive/formatter.h>
#include <lodepng/lodepng.h>
#include <iostream>
#include <stdint.h>

std::vector<char> formatter::formatPng(std::vector<char> file)
{
	unsigned char *result;
	uint32_t width, height;

	lodepng_decode_memory(&result, &width, &height, (unsigned char*)file.data(), file.size(), LCT_RGBA, 8);
	
	std::vector<char> pixels = std::vector<char>(result, result + width * height * 4);

	pixels.insert(pixels.begin(), (unsigned char*)&height, (unsigned char*)&height + 4);
	pixels.insert(pixels.begin(), (unsigned char*)&width, (unsigned char*)&width + 4);

	for(int i = 0; i < 8; ++i)
		std::cout << pixels.data()[i];
	std::cout << std::endl;

	return pixels;
}

std::vector<char> formatter::format(std::vector<char> file, const std::string extension)
{
	if(extension == "png")
		return formatPng(file);
	else
		return file;
}