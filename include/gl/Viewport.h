#pragma once

#include "Common.h"
#include "gl/GLCommon.h"

#include <memory>
#include <vector>

class Viewport {

	public:

		typedef std::shared_ptr<Viewport> Shared;
		static Shared alloc(
			uint32_t = 0,
			uint32_t = 0,
			uint32_t = 0,
			uint32_t = 0);

		Viewport(
			uint32_t = 0,
			uint32_t = 0,
			uint32_t = 0,
			uint32_t = 0);

		void resize(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		uint32_t x;
		uint32_t y;
		uint32_t width;
		uint32_t height;

		friend bool operator== (const Viewport::Shared&, const Viewport::Shared&);
		friend bool operator!= (const Viewport::Shared&, const Viewport::Shared&);

	private:

		// prevent copy-construction
		Viewport(const Viewport&);
		// prevent assignment
		Viewport& operator= (const Viewport&);
};
