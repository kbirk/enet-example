#pragma once

#include "Common.h"

#include <glm/glm.hpp>

enum class CommandType {
	MOVE
};

struct Command {
	CommandType type;
	glm::vec3 direction;
};
