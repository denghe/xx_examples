#pragma once
#include "pch.h"

struct Bullet {
	xx::XY pos{};
	int32_t lineNumber{};
	float radius{};

	void Hit(int hp);
	int32_t UpdateCore();
	void Update();			// fixed update
	void Draw();
};
