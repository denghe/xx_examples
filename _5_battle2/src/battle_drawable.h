#pragma once

namespace Battle {

	struct alignas(8) Drawable {
		xx::Frame* frame{};
		XY pos{};
		float radians{}, radius{};
		xx::RGBA8 color{ xx::RGBA8_White };
		float colorplus{ 1 };

		void Draw();
	};

}
