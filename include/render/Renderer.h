#pragma once

#include "Common.h"
#include "render/RenderCommand.h"

#include <vector>

namespace Renderer {

	void render(const RenderCommand::Shared& command);
	void render(const std::vector<RenderCommand::Shared>&);

}
