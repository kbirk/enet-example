#pragma once

#include "Common.h"

#include <string>
#include <vector>

std::vector<uint8_t> loadImage(
	uint32_t&,
	uint32_t&,
	const std::string&,
	bool = false);
