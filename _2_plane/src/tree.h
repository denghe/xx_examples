#pragma once
#include "pch.h"

struct Tree {
	xx::FromTo<xx::XY> aabb;
	int32_t lineNumber{};

	void Hit(int hp);
	int32_t UpdateCore();
	void Update();			// fixed update
	void Draw();
};
