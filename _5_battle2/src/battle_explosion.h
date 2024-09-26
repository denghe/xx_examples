#pragma once

namespace Battle {

	struct Explosion : Drawable {
		constexpr static float cRadius{ 15 };
		constexpr static float cFrameInc{ 18 / gLooper.fps };

		float frameIndex{};

		void Init(XY const& pos_, float scale_);
		bool Update();
	};

}
