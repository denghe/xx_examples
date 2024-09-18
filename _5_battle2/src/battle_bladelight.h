#pragma once

namespace Battle {

	struct BladeLight : Drawable {
		constexpr static float cRadius{ 32 };
		constexpr static float cFadeOutStep{ 1.f / 0.2f / Cfg::fps };

		float alpha{};
		int32_t lineNumber{};

		void Init(XY const& pos_, float radians_, float scale_);
		int32_t UpdateCore();
		bool Update();
	};

}
