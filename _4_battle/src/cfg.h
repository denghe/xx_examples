#pragma once
#include "pch.h"

struct Cfg : xx::GDesign<1920, 1080, 60> {
	static constexpr float maxItemSize{ 64 };
	static constexpr float maxItemSize_2{ maxItemSize / 2 };

	static constexpr int32_t physCellSize{ (int32_t)maxItemSize };
	static constexpr int32_t physNumRows{ int32_t(height * 2 / physCellSize) };
	static constexpr int32_t physNumCols{ int32_t(width * 2 / physCellSize) };

	static constexpr XY mapSize{ float(physNumCols * physCellSize), float(physNumRows * physCellSize) };
	static constexpr XY mapSize_2{ mapSize.x / 2, mapSize.y / 2 };

	static constexpr XY mapEdgeMin{ maxItemSize * 2, maxItemSize * 2 };
	static constexpr XY mapEdgeMax{ mapSize - mapEdgeMin };

	static constexpr XY screenSafeEdgeMin{ width_2 - maxItemSize_2, height_2 - maxItemSize_2 };
	static constexpr XY screenSafeEdgeMax{ width_2 + maxItemSize_2, height_2 + maxItemSize_2 };

	static constexpr xx::FromTo<float> mouseHitRadius{ 20, 500 };
};
