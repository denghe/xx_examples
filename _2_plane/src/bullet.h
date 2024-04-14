#pragma once
#include "cfg.h"

struct Bullet {
	constexpr static int32_t cLife{ 5 * (int32_t)gCfg.fps };
	constexpr static float cRadius{ 8 };
	constexpr static float cSpeed{ 500 / gCfg.fps };
	constexpr static int32_t cDamage{ 5 }, cMaxDamage{ 20 };

	xx::XY pos{};
	xx::XY inc{};
	int32_t damage{};
	int32_t life{};

	void Init(xx::XY const& bornPos, xx::XY const& inc_);
	int32_t UpdateCore();
	bool Update();
	void Draw();
};
