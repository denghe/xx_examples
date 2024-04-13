#pragma once
#include "pch.h"

struct Cfg : xx::GDesign<1280, 800> {
	static constexpr int32_t unitSizei{ 64 };
	static constexpr float unitSize{ (float)unitSizei };
	static constexpr float unitSize_2{ unitSize / 2 };
	static constexpr float _1_unitSizef{ 1.f / unitSize };
	static constexpr float maxItemSize{ 128 };	// tree
	static constexpr float gSQ = 0.70710678f;
};
inline Cfg gCfg;
