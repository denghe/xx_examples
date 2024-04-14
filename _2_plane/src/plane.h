#pragma once
#include "cfg.h"

struct Plane {
	constexpr static float cScale{ 4 };
	constexpr static float cRadius{ 16 };
	constexpr static float cSpeed{ 160 / gCfg.fps };
	constexpr static float cFrameMaxChangeRadian{ float(M_PI * 10 / gCfg.fps) };
	constexpr static float cFireDistance{ cRadius };

	xx::XY pos{};
	float radians{};

	void Init(xx::XY const& bornPos);
	bool Update();
	void Draw();
};
