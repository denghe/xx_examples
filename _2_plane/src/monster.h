#pragma once
#include "cfg.h"

struct Monster {
	constexpr static float cScale{ 1 }, cMaxScale{ 4 };
	constexpr static float cScaleSteps{ gCfg.fps };
	constexpr static float cRadius{ 8 };	// mapping to cScale
	constexpr static float cSpeed{ 80 / gCfg.fps };
	constexpr static float cFrameMaxIndex{ 6.f };
	constexpr static float cFrameInc{ 0.1f * 60 / gCfg.fps };
	constexpr static int cLife{ 30 * (int)gCfg.fps };
	constexpr static int cHP{ 100 };
	constexpr static int cHertStateLife{ int(10 * gCfg.fps / 60) };

	xx::XY pos{};
	int32_t lineNumber{};
	float frameIndex{}, radius{}, scale{}, colorPlus{ 1 };
	float tarScale{}, scaleStep{};
	int life{ cLife }, hp{ cHP }, hertStateLife{};
	xx::TaskGuard hertStateTG;

	void Hit(int damage, xx::XY const& bulletPos);

	void Init(xx::XY const& bornPos);
	int32_t UpdateCore();
	bool Update();
	void Draw();
};
