#pragma once
#include "pch.h"

struct Explosion {
	constexpr static float cScale{ 2 };
	constexpr static float cFrameMaxIndex{ 5.f };
	constexpr static float cFrameInc{ 0.1f };

	xx::XY pos{};
	float radians{};
	float scale{};
	float frameIndex{};

	void Init(xx::XY const& bornPos, float scale_);
	bool Update();
	void Draw();
};
