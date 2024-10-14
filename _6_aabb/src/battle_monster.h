#pragma once

namespace Battle {

	struct Scene;
	struct Monster : Drawable {
		static constexpr int32_t cNeighbourMaxCount { 7 };
		static constexpr float cTimeout{ 2 };
		static constexpr float cSpeed{ 3000.f / gLooper.fps };
		static constexpr float cRadius{ 32 };
		static constexpr xx::RGBA8 cColor{ xx::RGBA8_White };

		XY inc{};
		float timeout{};
		int32_t blocksLimitCount{};
		bool runawayMode{};


		void Init(XY const& pos_ = gLooper.mapSize_2);
		int32_t Update();
		bool FillCrossInc();			// cross: return true
		int32_t BlocksLimit();				// return true: limit fail. out of area
	};
}
