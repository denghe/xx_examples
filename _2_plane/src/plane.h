#pragma once
#include "pch.h"

struct Plane {
	xx::XY pos{};
	int32_t lineNumber{};
	float radians{};
	float radius{};

	int32_t UpdateCore();
	void Update();			// fixed update
	void Draw();
};
