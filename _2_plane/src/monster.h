#pragma once
#include "pch.h"

struct Monster {
	xx::XY pos{};
	int32_t lineNumber{};
	float radians{};
	float radius{};
	float frameIndex{};

	void Hit(int hp);
	int32_t UpdateCore();
	void Update();			// fixed update
	void Draw();
};
