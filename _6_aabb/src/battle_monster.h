#pragma once

namespace Battle {

	struct Scene;
	struct Monster : Drawable {
		static constexpr int32_t cNeighbourMaxCount { 9 };
		static constexpr float cSpeed{ 200.f / gLooper.fps };
		static constexpr float cRadius{ 32 };
		static constexpr xx::RGBA8 cColor{ xx::RGBA8_White };

		XY inc{}, crossInc{};
		bool runawayMode{};
		bool moving{};
		int32_t leftMoveStep{};

		void Init(XY const& pos_ = gLooper.mapSize_2);
		int32_t Update();
		bool FillCrossInc();			// cross: return true
		bool BlocksLimit();				// return true: limit fail. out of area
	};
}
