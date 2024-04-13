#pragma once
#include "cfg.h"

struct Plane {
	constexpr static float cRadius{ 32 };
	constexpr static float cSpeed{ 160 / gCfg.fps };
	constexpr static float cFrameMaxChangeRadian{ float(M_PI * 10 / gCfg.fps) };
	constexpr static float cFireDistance{ cRadius * 0.6f };

	xx::XY pos{};
	float radians{};

	void Init(xx::XY const& bornPos);
	int32_t UpdateCore();
	bool Update();
	void Draw();
};
