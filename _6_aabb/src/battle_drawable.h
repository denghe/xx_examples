#pragma once

namespace Battle {

	struct alignas(8) Drawable {
		void Draw();

		xx::Frame* frame{};
		XY pos{}, scale{ 1, 1 };
		float radians{};
		xx::RGBA8 color{ xx::RGBA8_White };
		float colorplus{ 1 };
		float alpha{ 1 };

		float radius{};	// not for draw
		// ...

	};

}
