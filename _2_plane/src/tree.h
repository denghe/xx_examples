#pragma once
#include "pch.h"

struct Tree {
	xx::FromTo<xx::XY> aabb;
	int32_t lineNumber{};

	void Init(xx::XY const& pos_);
	int32_t UpdateCore();
	void Update();			// fixed update
	void Draw();
};
