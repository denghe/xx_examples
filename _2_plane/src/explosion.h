#pragma once
#include "pch.h"

struct Explosion {
	xx::XY pos{};
	int32_t lineNumber{};
	float radians{};
	float radius{};
	float frameIndex{};

	int32_t UpdateCore();
	void Update();
	void Draw();
};
