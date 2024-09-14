#pragma once

namespace Battle {

	struct Explosion {
		constexpr static float cScale{ 2.2 };
		constexpr static float cFrameInc{ 0.3f };

		XY pos{};
		float radians{}, scale{}, frameIndex{};

		void Init(XY const& pos_, float scale_);
		bool Update();
	};

}
