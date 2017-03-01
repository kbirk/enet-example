#pragma once

#include "Common.h"

namespace Net {
	enum Types {
		CLIENT_INFO
	};
}

namespace Game {

	// num server steps per sec
	const uint32_t STEPS_PER_SEC = 10;

	// step duration in microseconds
	const std::time_t STEP_DURATION = (1.0 / STEPS_PER_SEC) * 1000000;

	// the interpolation delay for a client in microseconds.
	// 3x packet send rate interpolation
	const std::time_t INTERPOLATION_DELAY = STEP_DURATION * 3;
}
