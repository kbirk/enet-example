#include "game/Image.h"

#include "log/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#include <map>
#include <chrono>
#include <algorithm>
#include <iterator>

std::vector<uint8_t> loadImage(uint32_t& width, uint32_t& height, const std::string& path, bool invert) {
	int32_t BIT_DEPTH = 4;
	LOG_INFO("Loading image `" << path << "` file");
	int32_t w, h, depth;
	uint8_t* image = stbi_load(path.c_str(), &w, &h, &depth, BIT_DEPTH);
	if (image == nullptr) {
		LOG_ERROR("Error loading `" << path << "` file");
		return std::vector<uint8_t>();
	}
	width = w;
	height = h;
	if (invert) {
		// invert image
		std::vector<uint8_t> output(w * h * BIT_DEPTH);
		for (int32_t row=0; row<h; row++) {
			for (int32_t col=0; col<w; col++) {
				int32_t irow = (h - 1) - row;
				int32_t i = (row * w + col) * BIT_DEPTH;
				int32_t j = (irow * w + col) * BIT_DEPTH;
				output[j] = image[i];
				output[j+1] = image[i+1];
				output[j+2] = image[i+2];
				output[j+3] = image[i+3];
			}
		}
		stbi_image_free(image);
		return output;
	}
	std::vector<uint8_t> output(image, image + (w * h * BIT_DEPTH));
	stbi_image_free(image);
	return output;
}
