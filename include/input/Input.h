#pragma once

#include "Common.h"

#include <memory>

class Input {

	public:

		typedef std::shared_ptr<Input> Shared;

		Input();
		virtual ~Input();

	private:

		// prevent copy-construction
		Input(const Input&);
		// prevent assignment
		Input& operator= (const Input&);
};
