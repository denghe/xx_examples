#pragma once
#include "pch.h"

struct Tree {
	constexpr static xx::XY cOffset{ 32, 16 };
	constexpr static xx::XY cSize{ 36, 16 };
	constexpr static xx::XY cSize_2{ cSize / 2.f };

	xx::XY pos{};
	xx::FromTo<xx::XY> aabb{};

	void Init(xx::XY const& pos_);
	void Draw();
};
