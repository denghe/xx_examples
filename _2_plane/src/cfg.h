#pragma once
#include "pch.h"

struct Cfg : xx::GDesign<1280, 800> {
	static constexpr float unitSize{ 64 };
	static constexpr float gSQ = 0.70710678f;
};
inline Cfg gCfg;
